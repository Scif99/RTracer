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
public:
    //Constructor
    Sphere(const Vec3& centre, float radius, std::shared_ptr<Material> mat)
        :m_centre{centre}, m_radius{radius},  m_mat_ptr{std::move(mat)} {} //{ assert(m_radius_>0);}

    [[nodiscard]] constexpr Vec3 Centre() const noexcept {return m_centre;}
    [[nodiscard]] constexpr float Radius() const noexcept {return m_radius;}

    [[nodiscard]] std::optional<HitData> Hit(const Ray& r, float t_low, float t_high) const override;

    [[nodiscard]] AABB BoundingBox() const override {
        const auto min = Vec3{m_centre.X() - m_radius, m_centre.Y() - m_radius, m_centre.Z() - m_radius};
        const auto max = Vec3{m_centre.X() + m_radius, m_centre.Y() + m_radius, m_centre.Z() + m_radius}; 
        return AABB(min,max);
    }
};
