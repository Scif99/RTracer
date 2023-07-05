#pragma once

#include <array>
#include <cassert>
#include <memory>
#include <optional>

#include "hittable.h"
#include "vec3.h"
#include "ray.h"

/// @brief A triangle is defined by 3 vertices along with a normal vector
/// @brief Vertices must be specified in CCW order to maintain consistent normals.
/// @brief Note that the normal vector is not normalized on construction
class Triangle : public Hittable
{
    std::array<Point3,3> m_vertices;
    Norm3 m_normal;
    bool b_double_sided;
    std::shared_ptr<Material> mat_ptr;
public:

    //Special members
    ~Triangle() = default;
    Triangle() = delete;
     Triangle(const Triangle&) = default;
    Triangle& operator=(const Triangle&) = default;
    Triangle(Triangle&&) noexcept = default;
    Triangle& operator=(Triangle&&) noexcept = default;

    //Constructors
    Triangle(const Point3& a, const Point3& b, const Point3& c, std::shared_ptr<Material> material, bool double_sided = false) 
        : m_vertices{a,b,c}, m_normal{(Cross(b-a,c-a))}, mat_ptr{std::move(material)}, b_double_sided{double_sided} {}

    
    virtual std::optional<HitData> Hit(const Ray& r, float low, float high) const override;

    constexpr Point3 V_1() const noexcept { return m_vertices[0];}
    constexpr Point3 V_2() const noexcept { return m_vertices[1];}
    constexpr Point3 V_3() const noexcept { return m_vertices[2];}
};
