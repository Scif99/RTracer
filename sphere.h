#pragma once

#include <memory>
#include <optional>

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable
{
    float m_radius;
    Vec3 m_centre;
    std::shared_ptr<Material> m_mat_ptr;

public:

    //Special members
    ~Sphere() = default;
    Sphere() = delete;
    Sphere(const Sphere&) = default;
    Sphere& operator=(const Sphere&) = default;
    Sphere(Sphere&&) noexcept = default;
    Sphere& operator=(Sphere&&) noexcept = default;

    //Constructor
    Sphere(const Vec3& centre, float radius, std::shared_ptr<Material> mat)
        :m_centre{centre}, m_radius{radius},  m_mat_ptr{mat} {} //{ assert(m_radius_>0);}

    constexpr Vec3 centre() const noexcept {return m_centre;}
    constexpr float radius() const noexcept {return m_radius;}

    virtual std::optional<HitData> hit(const Ray& r, float t_low, float t_high) const override;

    //returns the (normalised) outwards normal for a given point on the surface of the sphere
    Vec3 outward_normal(const Ray& r, float t) const noexcept { return unit_vector(r.at(t) - m_centre);}
};

