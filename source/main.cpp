#include <cassert>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <numbers>
#include <numeric>
#include <vector>

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "light.h"
#include "material.h"
#include "math.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"
#include "utility.h"
#include "vec3.h"

static constexpr auto mat_eta{1.5f}; //refractive index of materials
static constexpr Color kBackGroundColor = Vec3(0.235294, 0.67451, 0.843137); //background color of the scene
static constexpr auto eps{0.001f}; //bias to prevent self-intersection


HittableList RandomScene() {

    using namespace std;

    HittableList world;
    
    const auto mat_ground = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.5f, 0.5f, 0.5f));
    world.Add(std::make_shared<Sphere>(Point3(0.f,-1000.f,0.f), 1000.f, mat_ground));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            const auto choose_mat = RNG::Get().GenerateFloat(0.f,1.f);
            const Point3 center(a + 0.9*RNG::Get().GenerateFloat(0.f,1.f), 0.2, b + 0.9*RNG::Get().GenerateFloat(0.f,1.f));

            if ((center - Point3(4.f, 0.2f, 0.f)).Length() > 0.9f) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8f) {
                    // diffuse
                    const auto albedo = Color::Random() * Color::Random();
                    sphere_material = make_shared<Material>(Material::MaterialType::DIFFUSE, albedo);
                    world.Add(make_shared<Sphere>(center, 0.2f, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    const auto albedo = Color::Random(0.5f, 1.f);
                    sphere_material = make_shared<Material>(Material::MaterialType::MIRROR, albedo);
                    world.Add(make_shared<Sphere>(center, 0.2f, sphere_material));
                } else {
                    // glass
                    const auto sphere_material = make_shared<Material>(Material::MaterialType::DIELECTRIC,Vec3(0.5f,0.5f,0.5f));
                    world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Material>(Material::MaterialType::DIELECTRIC,Color(0.2f,0.2f,0.2f));
    world.Add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Material>(Material::MaterialType::MIRROR,  Color(0.7, 0.6, 0.5));
    world.Add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}



// a recursive function that determines the color of a pixel 
Color RayColor(const Ray& ray, const HittableList& scene, const PointLight& light, float t_low, float t_high, int depth) {
    assert(t_low <  t_high);

    //No more rays to trace, return background color
    if(depth<=0) return kBackGroundColor; 

    auto hit_data{ scene.Hit(ray, t_low, t_high) };

    //The ray didn't intersect anything
    if(!hit_data) {return kBackGroundColor;}

    //Unwrap
    const auto& [hit_param, hit_point, hit_normal, mat_ptr] = hit_data.value();

    if(mat_ptr->m_type == Material::MaterialType::MIRROR)
    {
        const auto reflected_dir{ Reflected(ray.Direction(),hit_normal)};
        const auto reflected_ray = Ray{ hit_point, reflected_dir}; 
        const auto reflectance{ Fresnel(Norm3(ray.Direction()), hit_normal, mat_eta)}; //A measure of 'what % of the ray gets reflected'
        return reflectance * RayColor(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);
        //TODO add some ambient color/attenuation so shadows aren't pitch black?
    }

    // //Glassy (refractive) surface
    if(mat_ptr->m_type == Material::MaterialType::DIELECTRIC)
    {

        //There is always at least some amount of reflection, so we can compute the reflected ray immediately
        const auto reflected_dir = Norm3{Reflected(ray.Direction(), hit_normal)};
        const auto reflected_ray = Ray{reflected_dir, hit_point};

        //Did the intersection produce refraction?                            
        const auto refracted_dir = std::optional<Vec3>{Refracted(Norm3(ray.Direction()), hit_normal, mat_eta)};     

        //No refraction(TIR) so we can return early
        if(!refracted_dir) { 
            return RayColor(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);
        }   

        //There is refraction, so we can generate the refracted ray
        const auto refracted_ray = Ray{hit_point, refracted_dir.value()};  
                                                  
        //The Fresnel equations dictate "how much" of the light is refracted vs reflected
        //compute reflectance using schlick approximation 
        auto reflectance = Fresnel(Norm3(ray.Direction()), hit_normal, mat_eta);
        return reflectance * RayColor(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1) + 
                            (1 - reflectance) * RayColor(refracted_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);

    }

    //Shade diffuse surface using Blinn-Phong model    
    else 
    {

        //Check if ray is a shadow ray by generating a ray from the hit point and casting it to all light sources 
        //To avoid any self-intersections we add some bias to the shadow ray (the direction depends on whether ray hits inside or outside of surface)
        // Direction vector from intersection point to light source
        const auto light_dir = Norm3{light.position - hit_point};                                                                        
        const auto secondary_ray = Ray{ hit_point, light_dir};

        //It's ok if the shadow ray hits another object IF the light source is closer than the occluding object
        if(const auto param = scene.Hit( secondary_ray, eps, std::numeric_limits<float>::max()); param) //TODO Modify for multiple lights
        {
            //Is this occluding object closer than the light source?
            const auto vec_surf_to_light{light.position - hit_point};
            const auto vec_surf_to_occ{param.value().hit_point - ray.Origin()};
            if(vec_surf_to_occ.LengthSquared() < vec_surf_to_light.LengthSquared() )

            return Color(0.f,0.f,0.f); 
        }

        //Ambient 
        constexpr auto ambient_strength{0.1f};
        const auto ambient_light{ ambient_strength*light.intensity };

        //Diffuse 
        const auto diffuse_angle = std::max(0.f,Dot(hit_normal, light_dir));
        const auto diffuse_light = Color{light.intensity * diffuse_angle }; // * surface color?

        //Specular  
        const auto v = Norm3{-ray.Direction()};
        const auto h = Norm3{light_dir+v}; //vector that bisects the light direction and eye direction
        const auto spec_angle{ std::max(0.f, Dot(hit_normal, h))};
        const Color specular_color{light.intensity*(std::pow(spec_angle, mat_ptr->specular_exponent))};
        
        return diffuse_light*(mat_ptr->Kd) + specular_color * (mat_ptr->Ks); 
    }
    
}

int main()
{

    //Image dimensions (in pixels)
    constexpr auto aspect_ratio{16.f/9.f};
    constexpr auto image_width{900}; 
    constexpr auto image_height = static_cast<int>(static_cast<float>(image_width)/aspect_ratio);
    // Camera
    constexpr auto lookfrom = Vec3{13.f,2.f,3.f};
    constexpr auto lookat = Vec3{0.f,0.f,0.f};
    constexpr auto vup = Vec3{0.f,1.f,0.f};
    constexpr auto vfov{20.f};
    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    //---------------------
    //Add geometry to scene
    //-----------------------

    HittableList world = RandomScene();
    // auto mat_ground = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.8f, 0.8f, 0.f));
    // auto mat_center = std::make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.7f, 0.3f, 0.3f));
    // auto mat_left = std::make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.8f, 0.8f, 0.8f));
    // auto mat_right = std::make_shared<Material>(Material::MaterialType::MIRROR, Color(0.8f, 0.6f, 0.2f));

    // world.Add(std::make_shared<Sphere>(Point3( 0.f,-100.5f, -1.f), 100.f, mat_ground));
    // world.Add(std::make_shared<Sphere>(Point3( 0.f, 0.f, -1.f), 0.5f, mat_center));
    // world.Add(std::make_shared<Sphere>(Point3(-1.f, 0.f, -1.f), 0.5f, mat_left));
    // //world.Add(std::make_shared<Sphere>(Point3(-1.f, 0.f, -1.f), -0.4f, mat_left));
    // world.Add(std::make_shared<Sphere>(Point3( 1.f, 0.f, -1.f), 0.5f, mat_right));

    //Lights
    constexpr auto light = PointLight{Point3{0.f,70.f,20.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{10};
    constexpr auto max_depth{5};


    //---------------------
    //Draw image
    //--------------------
    std::ofstream out_file("image.ppm");
    if(!out_file) (std::cerr<<"error opening file\n;");


    out_file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = image_height-1; j >= 0;--j )
    {
        std::cerr<<"\rRows Remaining: " << j << ' '<<std::flush;
        for(int i=0;i < image_width;++i)
        {
            Color sum_col{0.f,0.f,0.f}; //Sum of color over all samples (likely to be greater than 1)
            for(auto s = 0; s < samples_per_pixel; ++s)
            {
                //Sample in a random area around pixel for antialiasing
                const auto u{(static_cast<float>(i) + RNG::Get().GenerateFloat(0.f,1.f)) / static_cast<float>(image_width-1)}; 
                const auto v{(static_cast<float>(j) + RNG::Get().GenerateFloat(0.f,1.f) )/ static_cast<float>(image_height-1)};
                const Ray r = cam.GetRay(u,v);
                sum_col +=RayColor(r, world, light, 0.f, std::numeric_limits<float>::max(), max_depth );
            }
            PrintColor(out_file,sum_col, samples_per_pixel);
        }

    }
    std::cerr<<"\nDone.\n";
    return 0;
}