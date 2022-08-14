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
    Sphere(const Sphere&) = default;
    Sphere& operator=(const Sphere&) = default;
    Sphere(Sphere&&) = default;
    Sphere& operator=(Sphere&&) = default;

    //Constructor
    constexpr Sphere(float r, Vec3 v)
        : m_centre_{v}, m_radius_{r} { assert(m_radius_>0);}


    constexpr Vec3 centre() const noexcept {return m_centre_;}
    constexpr float radius() const noexcept {return m_radius_;}

    constexpr virtual std::optional<float> isHit(const Ray& r, float& low, float& high) const override final;

    constexpr virtual Vec3 outward_normal(const Ray& r, float t) const noexcept override final;
};

constexpr std::optional<float> Sphere::isHit(const Ray& r, float& low, float& high) const
{
    //First we sub the parametric equation for a ray into the parametric equation for a circle.
    //The resulting equation is a quadratic in the parameter t. Thus a root exists iff t has two distinct roots.
    
    const auto A{dot(r.direction(),r.direction())};
    const auto B{2*dot(r.direction(),r.origin()-centre())};
    const auto C{dot(r.origin()-centre(),r.origin()-centre()) - (radius()*radius())};
    const auto discriminant = (B*B) - 4*A*C;

    if(discriminant <=0 )return {}; //no roots

    //Use discriminant to compute smaller value of t at closest intersection point
    float t = (-dot(r.direction(),r.origin() - centre()) - discriminant)/dot(r.direction(),r.direction());
    if(t > high || t < low) 
    {
        t = (-dot(r.direction(),r.origin() - centre()) + discriminant)/dot(r.direction(),r.direction());
        if(t > high || t < low)
        //update high here?
        return {};} 
    return t;
}

//Return the normalized normal at intersection point
constexpr Vec3 Sphere::outward_normal(const Ray& r, float t) const noexcept
{
    return unit_vector(r.at(t) - m_centre_);
}