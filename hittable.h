#pragma once

#include "ray.h"
#include <optional>

class Hittable
{
public:
    virtual std::optional<float> isHit(const Ray& ray, float& low, float& high) const = 0; //should return/modify parameter t?
    
    virtual Vec3 normal(const Ray& r, float t) const noexcept = 0;
};