#pragma once

#include <array>
#include <cassert>
#include <memory>
#include <optional>

#include "hittable.h"
#include "vec3.h"
#include "ray.h"

/*
- We can define a triangle by its three vertices, along with a normal vector
- Vertices must be specified in counter-clockwise order (CCW) in order to have consistent normals
- Note that the normal is not normalized by default. 
*/

class Triangle : public Hittable
{
    std::array<Point3,3> m_points_;
    Vec3 m_normal_;
    bool b_double_sided_;
    std::shared_ptr<Material> mat_ptr_;
public:

    //Special members
    ~Triangle() = default;
    Triangle() = delete;
     Triangle(const Triangle&) = default;
    Triangle& operator=(const Triangle&) = default;
    Triangle(Triangle&&) noexcept = default;
    Triangle& operator=(Triangle&&) noexcept = default;

    //Constructors
    constexpr Triangle(const Point3& a, const Point3& b, const Point3& c, bool double_sided = false) 
        : m_points_{a,b,c}, m_normal_{(cross(b-a,c-a))}, b_double_sided_{double_sided} {}

    
    virtual std::optional<HitData> hit(const Ray& r, float low, float high) const override;

    constexpr Point3 a() const noexcept { return m_points_[0];}
    constexpr Point3 b() const noexcept { return m_points_[1];}
    constexpr Point3 c() const noexcept { return m_points_[2];}
    
    constexpr Vec3 outward_normal(const Ray& r, float t) const noexcept {return unit_vector(m_normal_);};
};
