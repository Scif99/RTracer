#pragma once

#include <memory>
#include <optional>
#include <utility>

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"
/*
TO-DO Make a shared interface to remove code dupolcation?
*/

class Sphere : public Hittable
{
private:
    float m_radius;
    Point3 m_centre;
    std::shared_ptr<Material> m_mat_ptr;
    AABB m_aabb;
public:
    //Constructor
    Sphere(const Vec3& centre, float radius, std::shared_ptr<Material> mat)
        :m_centre{centre}, m_radius{radius},  m_mat_ptr{std::move(mat)}, m_aabb(Sphere::BoundingBox(centre,radius)) {} //{ assert(m_radius_>0);}

    constexpr Vec3 Centre() const noexcept {return m_centre;}
    constexpr float Radius() const noexcept {return m_radius;}

    virtual std::optional<HitData> Hit(const Ray& r, float t_low, float t_high) const override;

    static AABB BoundingBox(const Vec3& centre, float radius) {

        const auto min = Vec3{centre.X() - radius, centre.Y() - radius, centre.Z() - radius};
        const auto max = Vec3{centre.X() + radius, centre.Y() + radius, centre.Z() + radius}; 
        return AABB(min,max);
    }
};
