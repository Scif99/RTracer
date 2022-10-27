#pragma once

#include <memory>
#include <optional>

#include "hittable.h"

class Translate : public Hittable
{
private:
    std::shared_ptr<Hittable> m_ptr; //pointer to the untransformed hittable
    Vec3 m_offset;
public:
    ~Translate() = default;
    Translate(std::shared_ptr<Hittable> ptr, const vec3& offset)
    : m_ptr(ptr), m_offset(offset) {}


    virtual std::optional<HitData> hit(const Ray& ray, float t_low, float t_high) const override; 

    

};


/*
Note that a translation does not change the normal at the intersection
*/
std::optional<HitData> Translate::hit(const Ray& ray, float t_low, float t_high) const
{
    const auto moved_ray = Ray{ray.origin() - m_offset, r.direction(), r.time()};
    if(const auto data = m_ptr->hit(moved_ray, t_low, t_high); data)
    {
        data.value().hit_point+=offset;
        data = tmp_data.value(); //update data of closest intersection
    }

}