#pragma once

#include <optional>

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable
{
    Vec3 m_centre_;
    float m_radius_;
public:

    //Special members
    ~Sphere() = default;
    Sphere() = delete;
    constexpr Sphere(const Sphere&) = default;
    constexpr Sphere& operator=(const Sphere&) = default;
    constexpr Sphere(Sphere&&) noexcept = default;
    constexpr Sphere& operator=(Sphere&&) noexcept = default;

    //Constructor
    constexpr Sphere(float r, Vec3 v, std::optional<Color> col = {})
        : Hittable{col}, m_centre_{v}, m_radius_{r} { assert(m_radius_>0);}


    constexpr Vec3 centre() const noexcept {return m_centre_;}
    constexpr float radius() const noexcept {return m_radius_;}

    constexpr virtual std::optional<float> isHit(const Ray& r, float low, float high) const override final;

    constexpr virtual Vec3 outward_normal(const Ray& r, float t) const noexcept override final;
};

constexpr std::optional<float> Sphere::isHit(const Ray& r, float low, float high) const
{
    //First we sub the parametric equation for a ray into the parametric equation for a circle.
    //The resulting equation is a quadratic in the parameter t. Thus a root exists iff t has two distinct roots.
    
    const auto oc = r.origin() - m_centre_;
    const auto A{r.direction().length_squared()};
    const auto B{dot(r.direction(),oc)}; //technically half_b
    const auto C{oc.length_squared() - (m_radius_*m_radius_)};
    const auto discriminant = (B*B) - A*C;

    if(discriminant < 0 )return {}; //no roots

    //Use discriminant to compute smaller value of t at closest intersection point
    float t = (-B - sqrtf(discriminant))/A;
    if(t > high || t < low) 
    {
        //this root is outside the range, try other root
        t = (-B + sqrtf(discriminant))/A;
        if(t > high || t < low) 
        return {};
    } 
    return t;
}

//Return the unit normal at intersection point
constexpr Vec3 Sphere::outward_normal(const Ray& r, float t) const noexcept
{
    return unit_vector(r.at(t) - m_centre_);
}