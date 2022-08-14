#pragma once

#include "ray.h"
#include <optional>

class Hittable
{

protected:
    Hittable() = default;
public:

    virtual ~Hittable() = default;
    Hittable(const Hittable&) = delete;
    Hittable& operator=(const Hittable&) = delete;

    virtual std::optional<float> isHit(const Ray& ray, float& low, float& high) const = 0; //should return/modify parameter t?
    
    virtual Vec3 outward_normal(const Ray& r, float t) const noexcept = 0;


};