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
#include "triangle.h"
#include "utility.h"
#include "vec3.h"

HittableList random_scene() {

    HittableList world;

    using std::make_shared;

    const auto ground_material = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.5, 0.5, 0.5));

    world.add(std::make_shared<Sphere>(Point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RNG::get().generate_float(0.f,1.f);
            Point3 center(a + 0.9f*RNG::get().generate_float(0.f,1.f), 0.2f, b + 0.9f*RNG::get().generate_float(0.f,1.f));

            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8f) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = make_shared<Material>(Material::MaterialType::DIFFUSE, albedo);
                    world.add(make_shared<Sphere>(center, 0.2f, sphere_material));
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

    auto material1 = make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.4, 0.2, 0.1) );
    world.add(make_shared<Sphere>(Point3(0.f, 1.f, 0.f), 1.f, material1));

    auto material2 = make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4.f, 1.f, 0.f), 1.f, material2));

    auto material3 = make_shared<Material>(Material::MaterialType::MIRROR, Color(0.7, 0.6, 0.5));
    world.add(make_shared<Sphere>(Point3(4.f, 1.f, 0.f), 1.f, material3));

    return world;
}



// a recursive function that determines the color of a pixel 
Color ray_color(const Ray& ray, const HittableList& scene, const Light& light, float t_low, float t_high, int depth) {

    if(depth<=0) return Color(0.f);

    auto hit_data = scene.hit(ray, t_low, t_high);
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
    constexpr float ambient_strength{0.2f};
    const Color ambient_light = ambient_strength*light.intensity();

    if(hit_data.value().mat_ptr->m_type_ == Material::MaterialType::MIRROR && depth>0)
    {
        const auto reflected_dir{ unit_vector(reflected(unit_vector(ray.direction()), hit_data.value().hit_normal)) };
        const auto reflected_ray = Ray{ hit_data.value().hit_point, reflected_dir }; 
        const auto attenuation = Color{ hit_data.value().mat_ptr->m_albedo_ };

        return attenuation * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);
        //***SHOULD THIS ALSO ADD SOME AMBIENT LIGHT?****
    }

    if(hit_data.value().mat_ptr->m_type_ == Material::MaterialType::DIELECTRIC && depth>0)
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
        const auto reflected_ray = Ray{hit_data.value().hit_point, unit_vector(reflected(unit_direction, hit_data.value().hit_normal)) };
        const std::optional<Vec3> transmitted_dir = refracted(unit_direction, hit_data.value().hit_normal, refraction_ratio);

        if(!transmitted_dir)
        {
            return attenuation * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1);
        }

        const auto transmitted_ray = Ray{ hit_data.value().hit_point, transmitted_dir.value() };
        //compute reflectance using schlick approximation 
        auto r0{(1 - refraction_ratio) / (1 + refraction_ratio)};
        r0 *= r0;
        const auto reflectance{ r0 + (1-r0)*pow((1 - dot(-unit_direction, hit_data.value().hit_normal)), 5) };

        return attenuation * ( reflectance * ray_color(reflected_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1) + 
                            (1 - reflectance) * ray_color(transmitted_ray, scene, light, eps, std::numeric_limits<float>::max(), depth-1)  );

    }
    //Surface is diffuse
    //Shade using Blinn-Phong model
    else
    {
        //Check if ray is a shadow ray
        if(const auto param = scene.hit( Ray{ hit_data.value().hit_point, light_dir}, eps, std::numeric_limits<float>::max()); param)
        {
            //if an intersection occured then this ray is a shadow ray - return ambient component
            return ambient_light* hit_data.value().mat_ptr->m_albedo_;
        }


        //Diffuse 
        const float diff_angle = dot(hit_data.value().hit_normal, light_dir);
        const Color diffuse_light{light.intensity() * std::max(0.f, diff_angle)};

        //Specular
        constexpr float specular_strength{0.5f};
        const Vec3 v = unit_vector(- ray.direction());
        const Vec3 h{unit_vector(light_dir+v)};
        const float spec_angle{dot(hit_data.value().hit_normal, h)};
        constexpr auto phong_exp{2<<6}; //2^3 ~ 10
        const Color specular_light{specular_strength*(float)std::pow(std::max(0.f, spec_angle), phong_exp)};

        return (ambient_light + diffuse_light + specular_light)* hit_data.value().mat_ptr->m_albedo_;
    }
    
}

int main()
{

    //Image dimensions (in pixels)
    constexpr auto aspect_ratio{16.f/9.f};
    constexpr auto image_width{1200}; 
    constexpr auto image_height = static_cast<int>((float)image_width/aspect_ratio);

    //Lights
    constexpr Light light{Point3{0.f,1.f,1.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{1};
    constexpr auto max_depth{5};

    //---------------------
    //Add geometry to scene
    //-----------------------

    HittableList world;

    //Materials
    auto material_ground = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.8, 0.8, 0.0));
    auto diffuse = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.7, 0.3, 0.3));
    auto dielectric   = std::make_shared<Material>(Material::MaterialType::DIELECTRIC, Color(0.8, 0.8, 0.8));
    auto mirror  = std::make_shared<Material>(Material::MaterialType::MIRROR, Color(0.8, 0.6, 0.2));

    //left Sphere
    world.add(std::make_shared<Sphere>(Point3{-1.f,0.f,-1.f}, 0.5f, dielectric ));

    //middle Sphere
    world.add(std::make_shared<Sphere>(Point3{0.f,0.f,-1.f}, 0.5f, diffuse));

    //right Sphere
    world.add(std::make_shared<Sphere>(Point3{1.f,0.f,-1.f}, 0.5f, mirror));

    //ground 
    world.add(std::make_shared<Sphere>(Point3(0.f,-100.5f,-1.f), 100.f, material_ground ));

    // Camera

    Camera camera(90.f, aspect_ratio);

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
                const auto u{((float)i + RNG::get().generate_float(0.f,1.f)) / (float)(image_width-1)};
                const auto v{((float)j + RNG::get().generate_float(0.f,1.f) )/ (float)(image_height-1)};
                const Ray r = camera.get_ray(u,v);
                col +=ray_color(r, world, light, 0.f, std::numeric_limits<float>::max(), max_depth );
            }

            print_color(std::cout,col, samples_per_pixel);
            
        }

    }
    std::cerr<<"\nDone.\n";
    return 0;



}