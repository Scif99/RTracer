#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

#include <memory>
#include <optional>
#include <utility>

class Material;

/// @brief Stores information from a ray-object intersection that is needed for shading
struct HitData
{
    float hit_param; //ray parameter at intersection
    Point3 hit_point; //point of intersection
    Norm3 hit_normal; //OUTWARDS normal at intersection
    std::shared_ptr<Material> mat_ptr; //material
};

/// @brief Interface for any entity that a ray can intersect
class Hittable
{
public:
    virtual ~Hittable() = default;

    /// @brief Optionally returns data at an intersection
    virtual std::optional<HitData> Hit(const Ray& ray, float t_low, float t_high) const = 0; 
};

#endif