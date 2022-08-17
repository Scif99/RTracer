#pragma once

#include <array>
#include <cassert>

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
public:

    //Special members
    ~Triangle() = default;
    Triangle() = delete;
    constexpr Triangle(const Triangle&) = default;
    constexpr Triangle& operator=(const Triangle&) = default;
    constexpr Triangle(Triangle&&) noexcept = default;
    constexpr Triangle& operator=(Triangle&&) noexcept = default;

    //Constructors
    constexpr Triangle(const Point3& a, const Point3& b, const Point3& c,std::optional<Color> col = {}, bool double_sided = false) 
        : Hittable(col), m_points_{a,b,c}, m_normal_{(cross(b-a,c-a))}, b_double_sided_{double_sided} {}

    
    constexpr virtual std::optional<float> isHit(const Ray& r, float low, float high) const override final;

    constexpr Point3 a() const noexcept { return m_points_[0];}
    constexpr Point3 b() const noexcept { return m_points_[1];}
    constexpr Point3 c() const noexcept { return m_points_[2];}
    
    constexpr virtual Vec3 outward_normal(const Ray& r, float t) const noexcept override final;
};


constexpr std::optional<float> Triangle::isHit(const Ray& r, float low, float high) const
{
    if(dot(r.direction(),m_normal_)==0) {return {};} //ray and triangle are parallel

    //Write all coefficients of the matrix... (p. 78 in Shirley)
    //LHS
    const auto A{a().x() - b().x()};
    const auto B{a().y() - b().y()};
    const auto C{a().z() - b().z()};
    const auto D{a().x() - c().x()};
    const auto E{a().y() - c().y()};
    const auto F{a().z() - c().z()};
    const auto G{r.direction().x()};
    const auto H{r.direction().y()};
    const auto I{r.direction().z()};

    //RHS
    const auto J{a().x() - r.direction().x()};
    const auto K{a().y() - r.direction().y()};
    const auto L{a().z() - r.direction().z()};


    const auto M{A*(E*I-H*F) + B*(G*F-D*I) + C*(D*H-E*G)};
    assert(M!=0.f); //Avoid division by 0 errors

    const auto gamma{( I*(A*K-J*B) + H*(J*C-A*L) + G*(B*L-K*C) ) / M};
    if(gamma < 0.f || gamma > 1.f) {return {};}

    const auto beta{( J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H- E*G) )/ M};
    if(beta<0.f || beta > 1.f - gamma) {return {};}

    const auto t{-1.f*(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C)) / M};

    if(t > high || t < low) {return {};} //If parameter is outside the range, ignore it
    return t;

}

//Return normalised normal
constexpr Vec3 Triangle::outward_normal(const Ray& r, float t) const noexcept
{
    return unit_vector(m_normal_);
}