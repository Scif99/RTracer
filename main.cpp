#include <cmath>
#include <cassert>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory.h>
#include <numbers>
#include <numeric>
#include <thread>
#include <vector>

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "light.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "texture.h"
#include "triangle.h"
#include "utility.h"
#include "vec3.h"


//A basic scene containing a sphere with each different material
HittableList basic_scene()
{
    HittableList scene;

    //Materials
    const auto material_ground = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.8, 0.8, 0.0));
    const auto diffuse = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.7, 0.3, 0.3));
    const auto dielectric   = std::make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.8, 0.8, 0.8));
    const auto mirror  = std::make_shared<Material>(Material::MaterialType::MIRROR, Color(0.8, 0.6, 0.2));

    //left Sphere
    scene.add(std::make_shared<Sphere>(Point3{-1.f,0.f,-2.f}, 0.5f, dielectric ));

    //middle Sphere
    scene.add(std::make_shared<MovingSphere>(Point3{0.f,0.f,-2.f}, Point3{0.f,0.2f,-2.f}, 0.f, 1.f, 0.5f, diffuse));    //moving
    //scene.add(std::make_shared<Sphere>(Point3{0.f,0.f,-2.f}, 0.5f, diffuse)); //stationary

    //right Sphere
    scene.add(std::make_shared<Sphere>(Point3{1.f,0.f,-2.f}, 0.5f, mirror));

    //ground 
    scene.add(std::make_shared<Sphere>(Point3(0.f,-100.5f,-1.f), 100.f, material_ground ));

    //Camera cam(Point3(0.f,0.f,0.f), Point3(0.f,0.f,-1.f), Point3(0.f,1.f,0.f), 90, aspect_ratio);


    return scene;
}

//A scene containing a hollow sphere
HittableList hollow_scene()
{
    HittableList world;

    //Materials
    const auto material_ground = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.8, 0.8, 0.0));
    const auto material_center = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.1, 0.2, 0.5));
    const auto material_left   = std::make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.8, 0.8, 0.8));
    const auto material_right  = std::make_shared<Material>(Material::MaterialType::MIRROR, Color(0.8, 0.6, 0.2));

    world.add(std::make_shared<Sphere>(Point3( 0.f, -100.5f, -1.f), 100.f, material_ground));
    world.add(std::make_shared<Sphere>(Point3( 0.f,    0.f, -1.f),   0.5f, material_center));
    world.add(std::make_shared<Sphere>(Point3(-1.f,    0.f, -1.f),   0.5f, material_left));
    world.add(std::make_shared<Sphere>(Point3(-1.f,    0.f, -1.f), -0.45f, material_left));
    world.add(std::make_shared<Sphere>(Point3( 1.f,    0.f, -1.f),   0.5f, material_right));


    //const Camera cam(Point3(-2,2,1), Point3(0,0,-1), Vec3(0,1,0), 20, aspect_ratio);
    //constexpr auto light = Light{Point3{0.f,1.f,1.f}, Color{0.5f,0.5f,0.5f}};
    return world;
}

HittableList random_scene() {

    HittableList world;

    using std::make_shared;

    const auto ground_material = make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.5, 0.5, 0.5));
    //world.add(std::make_shared<Sphere>(Point3(0.f,-100.5f,-1.f), 100.f, ground_material ));

    world.add(std::make_shared<Sphere>(Point3(0.f,-1000.f,0.f), 1000.f, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RNG::get().generate_float(0.f,1.f);
            const auto center = Point3{ a + 0.9f*RNG::get().generate_float(0.f,1.f), 0.2f, b + 0.9f*RNG::get().generate_float(0.f,1.f) };

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8f) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = make_shared<Material>(Material::MaterialType::DIFFUSE, albedo);
                    const auto center2 = center + Vec3(0.f, RNG::get().generate_float(0.f,0.5), 0.f);
                    world.add(make_shared<MovingSphere>(center, center2, 0.f, 1.f, 0.2f, sphere_material));
                } else if (choose_mat < 0.95f) {
                    // metal
                    auto albedo = Color::random(0.5f, 1.f);
                    sphere_material = make_shared<Material>(Material::MaterialType::MIRROR, albedo);
                    world.add(make_shared<Sphere>(center, 0.2f, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Material>(Material::MaterialType::DIELECTRIC,Color(0.4, 0.2, 0.1) );
                    world.add(make_shared<Sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

     const auto material1 = make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.4, 0.2, 0.1) );
     world.add(make_shared<Sphere>(Point3(0.f, 1.f, -1.f), 1.f, material1));

     const auto material2 = make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.4, 0.2, 0.1));
     world.add(make_shared<Sphere>(Point3(-4.f, 1.f, -1.f), 1.f, material2));

     const auto material3 = make_shared<Material>(Material::MaterialType::MIRROR, Color(0.7, 0.6, 0.5));
     world.add(make_shared<Sphere>(Point3(4.f, 1.f, -1.f), 1.f, material3));

    return world;
}


// a recursive function that determines the color of a pixel 
Color ray_color(const Ray& ray, const HittableList& scene, const Light& light, float t_low, float t_high, int depth) {

    if(depth<=0) return Color(0.f);

    auto hit_data{ scene.hit(ray, t_low, t_high) };
    if(!hit_data)
    {
        //ray didn't intersect anything - return background color
        const auto t = 0.5f*(ray.direction().y() + 1.f);
        return (1.f-t)*Color(1.f, 1.f, 1.f) + t*Color(0.5f, 0.7f, 1.f);
    }

    //Extract from optional??
    const auto light_dir {unit_vector(light.position() - hit_data.value().hit_point)}; // (normalised)Vector from intersection point to light source
    constexpr auto eps{0.001f}; //prevent self intersection

    //Ambient 
    constexpr auto ambient_strength{0.2f};
    const auto ambient_light{ ambient_strength*light.intensity() };

    if(hit_data.value().mat_ptr->m_type == Material::MaterialType::MIRROR && depth>0)
    {
        const auto reflected_dir{ unit_vector(reflected(unit_vector(ray.direction()), hit_data.value().hit_normal)) };
        const auto reflected_ray = Ray{ hit_data.value().hit_point, reflected_dir, ray.time() }; 
        const auto attenuation = Color{ hit_data.value().mat_ptr->m_albedo->getColor(hit_data.value().tex_coords) }; //query color from texture using texture coordinates

        return attenuation * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);
        //***SHOULD THIS ALSO ADD SOME AMBIENT LIGHT?****
    }

    if(hit_data.value().mat_ptr->m_type == Material::MaterialType::DIELECTRIC && depth>0)
    {
    
        //for now assume all dielectrics have the same refractive index.
        const auto mat_eta{1.5f};
        const auto attenuation = Color{1.f};
        const auto unit_direction{ unit_vector(ray.direction()) };
        const auto outside  = dot(unit_direction, hit_data.value().hit_normal) < 0.f ? true : false;
        const auto refraction_ratio = outside ? (1.f/mat_eta) : mat_eta;

        //the incident ray might be 'inside' the shape.
        //we can use the same equations for refraction, so long as we flip the normal in this case.
        if(!outside) {hit_data.value().hit_normal = -hit_data.value().hit_normal;} 


        //compute the direction of the transmitted/reflected ray
        const auto reflected_ray = Ray{hit_data.value().hit_point, 
        unit_vector(reflected(unit_direction, hit_data.value().hit_normal)), ray.time() };
        const std::optional<Vec3> transmitted_dir = refracted(unit_direction, hit_data.value().hit_normal, refraction_ratio);

        if(!transmitted_dir) return attenuation * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);

        const auto transmitted_ray = Ray{ hit_data.value().hit_point, transmitted_dir.value(), ray.time() };

        //compute reflectance using schlick approximation 
        auto r0{(1 - refraction_ratio) / (1 + refraction_ratio)};
        r0 *= r0;
        const auto reflectance{ r0 + (1-r0)*pow((1 - dot(-unit_direction, hit_data.value().hit_normal)), 5) };

        return attenuation * ( reflectance * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1) + 
                            (1 - reflectance) * ray_color(transmitted_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1)  );

    }

    //Shade diffuse surface using Blinn-Phong model
    else
    {
        //Check if ray is a shadow ray
        if(const auto param = scene.hit( Ray{ hit_data.value().hit_point, light_dir, ray.time()}, eps, std::numeric_limits<float>::max()); param)
        {
            //if an intersection occured then this ray is a shadow ray - return ambient component
            return ambient_light* hit_data.value().mat_ptr->m_albedo->getColor(hit_data.value().tex_coords); //query color from texture using texture coordinates
        }


        //Diffuse 
        const auto diff_angle{ dot(hit_data.value().hit_normal, light_dir) };
        const auto diffuse_light = Color{light.intensity() * std::max(0.f, diff_angle)};

        //Specular
        constexpr auto specular_strength{0.5f};
        const auto v{ unit_vector(- ray.direction()) };
        const auto h{ unit_vector(light_dir+v) };
        const auto spec_angle{ dot(hit_data.value().hit_normal, h)};
        const auto phong_exp{2<<6}; //2^3 ~ 10
        const Color specular_light{specular_strength*(float)std::pow(std::max(0.f, spec_angle), phong_exp)};
        
        //query color from texture using texture coordinates
        return (ambient_light + diffuse_light + specular_light)* hit_data.value().mat_ptr->m_albedo->getColor(hit_data.value().tex_coords); 
    }
    
}

int main()
{

    //Image dimensions (in pixels)
    constexpr auto aspect_ratio{16.f/9.f};
    constexpr auto image_width{400}; 
    constexpr auto image_height = static_cast<int>((float)image_width/aspect_ratio);

    //Lights
    constexpr auto light = Light{Point3{0.f,10.f,0.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{2000};
    constexpr auto max_depth{3};

    //---------------------
    //Add geometry to scene
    //-----------------------

    //HittableList world = random_scene();
    HittableList world;
    auto spotted_texture = std::make_shared<ImageTexture>("ball_spotted.jpg");
    auto spotted_surface = std::make_shared<Material>(Material::MaterialType::DIFFUSE, spotted_texture );
    auto spotted = std::make_shared<MovingSphere>(Point3(-1.f,0.f,-3.f), Point3(-0.8f,0.f,-3.f), 0.f, 1.f, 1.f, spotted_surface);
    world.add(spotted);

    auto striped_texture = std::make_shared<ImageTexture>("ball_striped.jpg");
    auto striped_surface = std::make_shared<Material>(Material::MaterialType::DIFFUSE, striped_texture );
    auto striped = std::make_shared<Sphere>(Point3(1.f,0.f,-3.f), 1.f, striped_surface);
    world.add(striped);


    // Camera
    Point3 lookfrom(13.f,2.f,3.f);
    Point3 lookat(0.f,0.f,0.f);
    Vec3 vup(0.f,1.f,0.f);
    float vfov = 45;

    lookfrom = Point3(0.f,3.f,5.f);
    lookat = Point3(0.f,0,0);

    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio);

    //---------------------
    //Draw image
    //--------------------

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = image_height-1; j >= 0;--j )
    {
        std::cerr<<"\rRows Remaining: " << j << ' '<<std::flush;
        for(int i=0;i < image_width;++i)
        {
            Color col{0.f,0.f,0.f};
            for(auto s = 0; s < samples_per_pixel; ++s)
            {
                const auto u{(i + RNG::get().generate_float(0.f,1.f)) / (image_width-1)};
                const auto v{((float)j + RNG::get().generate_float(0.f,1.f) )/ (float)(image_height-1)};
                const Ray r = cam.get_ray(u,v, 0.f,1.f);
                col +=ray_color(r, world, light, 0.f, std::numeric_limits<float>::max(), max_depth );
            }

            print_color(std::cout,col, samples_per_pixel);
            
        }

    }
    std::cerr<<"\nDone.\n";
    return 0;



}