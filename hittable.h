#pragma once

#include "ray.h"
#include <optional>
#include <tuple>

/*
- A hittable represents anything that a ray can intersect with
- A hittable may have a predetermined color associated with it
*/

class Hittable
{
    const std::optional<Color> op_color_;
protected:
    constexpr  Hittable(std::optional<Color> col): op_color_{col} {} // = default;
    
public:
    using hit_data = std::tuple<float, Point3, Vec3>;

    virtual ~Hittable() = default;
    Hittable(const Hittable&) = delete;
    Hittable& operator=(const Hittable&) = delete;

    constexpr virtual std::optional<float> isHit(const Ray& ray, float low, float high) const = 0; //should return/modify parameter t?
    constexpr virtual Vec3 outward_normal(const Ray& r, float t) const noexcept = 0;

    const std::optional<Color> color() const {return op_color_;}
};