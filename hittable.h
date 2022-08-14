#pragma once

#include "ray.h"

class Hittable
{
public:
    virtual bool isHit(const Ray& ray, float& low, float& high) const = 0; //should return/modify parameter t?
    
    virtual Color color() const noexcept = 0;
};