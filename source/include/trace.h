#ifndef TRACE_H
#define TRACE_H

#include "bvh.h"
#include "vec3.h"
#include "hittable_list.h"
#include "math.h"
#include "material.h"

//sorry
static constexpr auto mat_eta{1.5f}; //refractive index of materials
static constexpr Color kBackGroundColor = Vec3(0.235294, 0.67451, 0.843137); //background color of the scene
static constexpr auto eps{0.001f}; //bias to prevent self-intersection


// Algorithm.
Color RayColor(const Ray& ray, Hittable* scene, const PointLight& light, float t_low, float t_high, int depth) {
    assert(t_low <  t_high);

    //No more rays to trace, return background color
    if(depth<=0) return kBackGroundColor; 

    auto hit_data{ scene->Hit(ray, t_low, t_high) };

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
        if(const auto param = scene->Hit( secondary_ray, eps, std::numeric_limits<float>::max()); param) //TODO Modify for multiple lights
        {
            //Is this occluding object closer than the light source?
            const auto vec_surf_to_light{light.position - hit_point};
            const auto vec_surf_to_occ{param.value().hit_point - ray.Origin()};
            if(vec_surf_to_occ.LengthSquared() < vec_surf_to_light.LengthSquared() )

            return Color(0.f,0.f,0.f); 
        }

        //Ambient 
        // constexpr auto ambient_strength{0.1f};
        // const auto ambient_light{ ambient_strength*light.intensity };

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

#endif