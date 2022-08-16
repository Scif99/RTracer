#pragma once

#include "ray.h"
#include <optional>


/*
A hittable represents anything that a ray can intersect with
*/

class Hittable
{

protected:
    constexpr Hittable(bool m): is_mirror{m} {} // = default;
public:

    virtual ~Hittable() = default;
    Hittable(const Hittable&) = delete;
    Hittable& operator=(const Hittable&) = delete;

    constexpr virtual std::optional<float> isHit(const Ray& ray, float& low, float& high) const = 0; //should return/modify parameter t?
    constexpr virtual Vec3 outward_normal(const Ray& r, float t) const noexcept = 0;
    bool is_mirror;

};