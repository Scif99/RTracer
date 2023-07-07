#include <cassert>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <numbers>
#include <numeric>
#include <vector>

#include "bvh.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "light.h"
#include "material.h"
#include "math.h"
#include "ray.h"
#include "sphere.h"
#include "trace.h"
#include "triangle.h"
#include "utility.h"
#include "vec3.h"


HittableList RandomScene() {
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
                    sphere_material = std::make_shared<Material>(Material::MaterialType::DIFFUSE, albedo);
                    world.Add(std::make_shared<Sphere>(center, 0.2f, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    const auto albedo = Color::Random(0.5f, 1.f);
                    sphere_material = std::make_shared<Material>(Material::MaterialType::MIRROR, albedo);
                    world.Add(std::make_shared<Sphere>(center, 0.2f, sphere_material));
                } else {
                    // glass
                    const auto sphere_material = std::make_shared<Material>(Material::MaterialType::DIELECTRIC,Vec3(0.5f,0.5f,0.5f));
                    world.Add(std::make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Material>(Material::MaterialType::DIELECTRIC,Color(0.2f,0.2f,0.2f));
    world.Add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Material>(Material::MaterialType::DIFFUSE, Color(0.4, 0.2, 0.1));
    world.Add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Material>(Material::MaterialType::MIRROR,  Color(0.7, 0.6, 0.5));
    world.Add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
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
    auto root = std::make_unique<BVHNode>(world);
    
    //Lights
    constexpr auto light = PointLight{Point3{0.f,70.f,20.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{50};
    constexpr auto max_depth{10};


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
                sum_col +=RayColor(r, root.get(), light, 0.f, std::numeric_limits<float>::max(), max_depth );
            }
            PrintColor(out_file,sum_col, samples_per_pixel);
        }

    }
    std::cerr<<"\nDone.\n";
    return 0;
}