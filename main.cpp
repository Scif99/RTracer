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

//Function that returns data from the closest collision between a ray and the scene objects
std::optional<hit_record> closest(const Ray& ray, const std::vector<std::unique_ptr<Hittable>>& hittables, float low, float high)
{
    std::optional<hit_record> data;
    for(const auto& surface : hittables)
    {
        if(const auto param = surface->isHit(ray,low,high); param)
        {
            high = param.value(); //update the max range for t if ray hits something
            data = std::make_tuple(param.value(), //ray parameter at intersection
                                   ray.at(param.value()), //point of intersection
                                   unit_vector(surface->outward_normal(ray,param.value())), //unit normal at intersection 
                                   surface.get()); //pointer to the surface object
        }
    }
    return data;
}


Color ray_color(const Ray& ray, const std::vector<std::unique_ptr<Hittable>>& hittables, const Light& light, int depth, float low, float high) {

    if(depth==0) return Color(0.f); //No more rays generated i.e no more color gathered

    const auto hit_data = closest(ray,hittables, low, high); //get data at closest intersection

    //ray did not intersect anything - return background color
    if(!hit_data) 
    {
        const auto t = 0.5f*(ray.direction().y() + 1.f);
        return (1.f-t)*Color(1.f, 1.f, 1.f) + t*Color(0.5f, 0.7f, 1.f);
    }


    const auto [hit_param, hit_point, hit_normal, surface] = hit_data.value();
    const Color surface_color = surface->color() ? surface->color().value() : 0.5*(hit_normal+ Vec3(1.f,1.f,1.f)); 
    const Vec3 light_dir {unit_vector(light.position() - hit_point)}; //Vector from intersection point to light source
    constexpr auto eps{0.001f}; //prevent self intersection

    //Ambient 
    constexpr Color ambient_light(0.2f);


    if(surface->is_dielectric)
    {
        const auto reflected_vec {reflected(ray.direction(), hit_normal)};
        std::optional<Vec3> refracted_vec;

        //For now assume all dielectrics have the same refractive index.
        constexpr auto air_eta{1.f};
        constexpr auto mat_eta{1.5f};

        float c; //the angle between the incomig ray and surface normal
        auto k = Color{0.f};


        if(dot(ray.direction(),hit_normal) < 0) //If incoming ray is 'outside' surface.
        {
            refracted_vec = refracted(ray.direction(), hit_normal, air_eta, mat_eta); // assert: refracted !=null;
            c = -dot(ray.direction(),hit_normal);
            k = Color{1.f,1.f,1.f}; //attenuation
        }
        
        else //Incoming ray is 'inside'
        {
            k = Color{0.1f,0.1f,0.1f}; //?
            if(const auto ref = refracted(ray.direction(), - hit_normal, mat_eta, air_eta); ref) //TIR did not occur
            {
                refracted_vec = ref;
                c  = dot(unit_vector(refracted_vec.value()),hit_normal);
            }
            else //TIR
            {
                return k*ray_color(Ray(hit_point,reflected_vec),hittables,light, depth-1, eps, std::numeric_limits<float>::max());
            }

        }

        const auto R_0 = ((mat_eta-1.f)*(mat_eta-1.f)) / ((mat_eta+1.f)*(mat_eta+1.f));
        const auto R = R_0 + ((1 - R_0)*pow((1 - c),5));

        return  k* (R*ray_color(Ray(hit_point,reflected_vec),hittables,light, depth-1, eps, std::numeric_limits<float>::max()) 
                + (1.f-R) *ray_color(Ray(hit_point,refracted_vec.value()),hittables,light, depth-1, eps, std::numeric_limits<float>::max()));

    }


    if(surface->is_mirror)
    {
        return ambient_light*surface_color + 
               0.3f*ray_color(Ray(hit_point, reflected(ray.direction(), hit_normal)),hittables,light, depth-1, eps, std::numeric_limits<float>::max());
    }

    //Surface is diffuse
    //Shade using Blinn-Phong model
    else
    {
        //Check for a shadow ray
        for(const auto& other: hittables)
        {
            //Use eps to prevent self intersection
            if(const auto param = other->isHit(Ray{hit_point,light_dir},eps,std::numeric_limits<float>::max()); param) 
            {
                return ambient_light*surface_color;
            }
        }

        //Diffuse 
        const Color diffuse_light{light.intensity()*std::max(0.f,dot(hit_normal,light_dir))};

        //Specular
        constexpr Color specular_color{0.5f,0.5f,0.5f};
        const Vec3 v = unit_vector(- ray.direction());
        const Vec3 h{unit_vector(light_dir+v)};
        constexpr auto ph_exp{2<<6}; //2^3 ~ 10
        const Color specular_light{specular_color*std::pow(std::max(0.f,dot(hit_normal,h)),ph_exp)};
        return (ambient_light*surface_color + diffuse_light+specular_light)*surface_color;
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


    //Left sphere
    v_hittables.push_back(std::make_unique<Sphere>(0.5f,Point3{-1.f,0.f,-1.f}, Color(0.f, 0.f, 0.f)));
    v_hittables.back()->is_dielectric = true;

    //Centre Sphere
    v_hittables.push_back(std::make_unique<Sphere>(0.5f,Point3{0.f,0.f,-1.f}, Color(0.7f, 0.3f, 0.3f)));


    //Right
    v_hittables.push_back(std::make_unique<Sphere>(0.5f,Point3{1.f,0.f,-1.f}, Color(0.8, 0.6, 0.2)));
    v_hittables.back()->is_mirror = true;

    //Can model a 'floor' with two triangles
    constexpr Color floor_color{0.6f,0.6f,0.6f};
    v_hittables.push_back(std::make_unique<Triangle>(Point3(-100.f,-1.f,100.f),Point3(100.f,-1.f,100.f),Point3(100.f,-1.f,-100.f),floor_color));

    //MAKE THE FLOOR A MIRROR
    //v_hittables.back()->is_mirror = true;
    v_hittables.push_back(std::make_unique<Triangle>(Point3(-100.f,-1.f,100.f),Point3(100.f,-1.f,-100.f),Point3(-100.f,-1.f,-100.f),floor_color));
    //v_hittables.back()->is_mirror = true;

    //Lights
    constexpr Light light{Point3{0.f,1.f,1.f}, Color{0.5f,0.5f,0.5f}};
    constexpr auto samples_per_pixel{100};
    constexpr auto max_depth{2};

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
                col +=ray_color(r,v_hittables, light,max_depth, 0.f, std::numeric_limits<float>::max() );
            }

            print_color(std::cout,col, samples_per_pixel);
            
        }
    }
    std::cerr<<"\nDone.\n";
    return 0;



}