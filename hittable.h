#pragma once

#include "ray.h"

#include <memory>
#include <optional>
#include <tuple>

/*
- A hittable represents anything that a ray can intersect with
- A hittable may have a predetermined color associated with it
*/

class Material;

struct HitData
{
    float hit_param; //ray parameter at intersection
    Point3 hit_point; //point of intersection
    Vec3 hit_normal; //(outwards) normal at intersection
    std::shared_ptr<Material> mat_ptr; //material
};

class Hittable
{
public:
    virtual ~Hittable() = default;
    Hittable() = default;
    Hittable (const Hittable&) = default;
    Hittable& operator=(const Hittable&) = delete;

    //return data at intersection, if one occured
    virtual std::optional<HitData> hit(const Ray& ray, float t_low, float t_high) const = 0; 

};