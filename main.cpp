#include <cmath>
#include <cassert>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory.h>
#include <numeric>
#include <thread>
#include <vector>

#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "triangle.h"
#include "utility.h"
#include "light.h"



using hit_record = std::tuple<float,Point3, Vec3, Hittable*>;

//Function that return
std::optional<hit_record> closest(const Ray& r, const std::vector<std::unique_ptr<Hittable>>& hittables)
{
    float low =0;
    float high = std::numeric_limits<float>::max();
    std::optional<hit_record> data;
    for(const auto& surface : hittables)
    {
        if(const auto t = surface->isHit(r,low,high); t)
        {
            high = t.value();
            data = std::make_tuple(t.value(),r.at(t.value()),unit_vector(surface->outward_normal(r,t.value())), surface.get()); //update data at closest intersection
        }
    }
    return data;

}

Color ray_color(const Ray& r, const std::vector<std::unique_ptr<Hittable>>& hittables, const Light& light, int depth) {
    
    const auto hit_data = closest(r,hittables); //get data at closest intersection

    if(!hit_data) //ray did not intersect anything
    {
        //Compute background color
        const Vec3 unit_direction{unit_vector(r.direction())}; //constexrp?
        const auto t = 0.5f*(unit_direction.y() + 1.f);
        return (1.f-t)*Color(1.f, 1.f, 1.f) + t*Color(0.5f, 0.7f, 1.f);
    }

    const auto& [t,p,n,s] = hit_data.value();
    const Color surface_color = s->color() ? s->color().value() : 0.5*(n+ Vec3(1.f,1.f,1.f)); 

    if(dot(r.direction(),n) < 0.f) //outside
    {
        const Vec3 l {unit_vector(light.position()-p)}; //Vector from intersection point to light source
        constexpr auto eps{0.5f}; //prevent self intersection

        //Ambient 
        constexpr Color ambient_light(0.2f);

        //If outgoing ray from intersection point hits some other surface, then it must correspond to a shadow ray
        for(const auto& other: hittables)
        {
            if(const auto param = other->isHit(Ray{p,l},eps,std::numeric_limits<float>::max()); param) 
            {
                return ambient_light*surface_color;
            }
        }

        //Diffuse 
        const Color diffuse_light{light.intensity()*std::max(0.f,dot(n,l))};

        //Specular
        constexpr Color specular_color{0.5f,0.5f,0.5f};
        const Vec3 v = unit_vector(- r.direction());
        const Vec3 h{unit_vector(l+v)};
        constexpr auto p{2<<9}; //2^3 ~ 10
        const Color specular_light{specular_color*std::pow(std::max(0.f,dot(n,h)),p)};
        return (ambient_light + diffuse_light+specular_light)*surface_color;
        //return Color(0.5*(h->outward_normal(r,t.value())+ Vec3(1.f,1.f,1.f))); //Color according to the normal vector...

    }

    else  //inside    **double sided triangles?
    {
        return Color(1.f,0.f,0.f);
    }
}

int main()
{
    //Image dimensions
    constexpr auto aspect_ratio{16.f/9.f};
    constexpr auto image_width{800}; //no. of pixels...
    constexpr auto image_height = static_cast<int>((float)image_width/aspect_ratio);
    //Camera
    constexpr auto view_height{2.f};
    constexpr auto view_width{(float)view_height*aspect_ratio};
    constexpr auto focal_length{1.f}; //distance from viewpoint to image

    constexpr auto viewpoint = Point3{0.f,0.f,0.f}; //'eye' point
    constexpr auto view_direction = Vec3{0.f,0.f,-focal_length}; //Direction of image plane

    constexpr auto vertical = Vec3{0.f,view_height,0.f};
    constexpr auto horizontal = Vec3{view_width,0.f,0.f};
    constexpr auto lower_left = viewpoint - horizontal/2.f - vertical/2.f + view_direction; 

    std::vector<std::unique_ptr<Hittable>> v_hittables;
    v_hittables.push_back(std::make_unique<Sphere>(0.5f,Point3{0.f,0.f,-2.f}));
    //v_hittables.push_back(std::make_unique<Triangle>(Point3(0.f,4.f,-5.f),Point3(-4.f,-1.f,-5.f),Point3(4.f,-1.f,-5.f))); //Make sure to specify in CCW

    //Can model a 'floor' with two triangles
    constexpr Color floor_color{0.5f,0.5f,0.5f};
    v_hittables.push_back(std::make_unique<Triangle>(Point3(-100.f,-1.f,0.f),Point3(100.f,-1.f,-0.f),Point3(100.f,-1.f,-100.f),floor_color));
    v_hittables.push_back(std::make_unique<Triangle>(Point3(-100.f,-1.f,0.f),Point3(100.f,-1.f,-100.f),Point3(-100.f,-1.f,-100.f),floor_color));

    constexpr Light light{Point3{0.f,1.f,1.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{100};
    constexpr auto max_depth{10};

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = image_height-1; j >= 0;--j )
    {
        std::cerr<<"\rRows Remaining: " << j << ' '<<std::flush;
        for(int i=0;i < image_width;++i)
        {
            Color col{0.f,0.f,0.f};
            for(auto s = 0;s<samples_per_pixel;++s)
            {
                const auto u{((float)i + RNG::get().generate_float(0.f,1.f)) / (float)(image_width-1)};
                const auto v{((float)j + RNG::get().generate_float(0.f,1.f) )/ (float)(image_height-1)};
                const Ray r{viewpoint, lower_left - viewpoint + u*horizontal + v*vertical}; //Generate the ray from viewpoint to pixel location
                col +=ray_color(r,v_hittables, light,max_depth);
            }

            print_color(std::cout,col, samples_per_pixel);
            
        }
    }
    std::cerr<<"\nDone.\n";
    return 0;



}