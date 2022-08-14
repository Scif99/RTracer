#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <cassert>
#include <memory.h>
#include <numeric>

#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "triangle.h"


Color ray_color(const Ray& r, const std::vector<std::unique_ptr<Hittable>>& hittables) {

    //We only need to find the first object that intersects with the ray (if any).
    auto low = 0.f;
    auto high = std::numeric_limits<float>::max();
    for(const auto& h :  hittables) //Iterate through all the geometry in the scene
    {
        if(auto t = h->isHit(r,low,high); t)
        {
            return Color(0.5*(h->normal(r,t.value())+ Vec3(1.f,1.f,1.f))); //Color according to the normal vector...
        }
    }
    const Vec3 unit_direction{unit_vector(r.direction())}; //constexrp?
    const auto t = 0.5f*(unit_direction.y() + 1.f);
    return (1.f-t)*Color(1.f, 1.f, 1.f) + t*Color(0.5f, 0.7f, 1.f);
}


int main()
{
    //Image dimensions
    constexpr auto aspect_ratio{16.f/9.f};
    constexpr auto image_width{400}; //no. of pixels...
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
    v_hittables.push_back(std::make_unique<Sphere>(0.5f,Vec3{0.f,0.f,-1.f}));
    v_hittables.push_back(std::make_unique<Triangle>(Point3(0.f,4.f,-2.f),Point3(4.f,0.f,-2.f),Point3(-4.f,0.f,-2.f)));


    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = image_height-1; j >= 0;--j )
    {
        //std::cerr<<"\rRows Remaining: " << j <<std::flush;
        for(int i=0;i < image_width;++i)
        {
            const auto u{(float)i / (image_width-1)};
            const auto v{(float)j / (image_height-1)};
            const Ray r{viewpoint, lower_left - viewpoint + u*horizontal + v*vertical}; //Generate the ray from viewpoint to pixel location

            const Color c = ray_color(r, v_hittables);
            print_color(std::cout,c);
            
        }
    }
    //std::cerr<<"\nDone.\n";
    return 0;



}