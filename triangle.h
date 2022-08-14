#pragma once

#include <array>

#include "hittable.h"
#include "vec3.h"
#include "ray.h"

/*
- We can define a triangle by its three vertices, along with a normal vector

*/

class Triangle : public Hittable
{
public:
    constexpr Triangle(const Point3& a, const Point3& b, const Point3& c) //points should be specified in counter-clockwise order?
        : m_points_{a,b,c}, m_normal_{cross(b-a,c-a)} {}

    

    //Overloads for rvalues?
    
    virtual bool isHit(const Ray& r, float& low, float& high) const override final;

    constexpr Point3 a() const noexcept { return m_points_[0];}
    constexpr Point3 b() const noexcept { return m_points_[1];}
    constexpr Point3 c() const noexcept { return m_points_[2];}
    
    virtual Color color() const noexcept {return m_color_;}

private:
    std::array<Point3,3> m_points_;
    Vec3 m_normal_;

    inline static const Color m_color_{1.f,0.f,0.f};
};


bool Triangle::isHit(const Ray& r, float& low, float& high) const
{

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

    const auto gamma{( I*(A*K-J*B) + H*(J*C-A*L) + G*(B*L-K*C) ) / M};
    if(gamma < 0.f || gamma > 1.f) {return false;}

    const auto beta{( J*(E*I - H*F) + K*(G*F - D*I) + L*(D*H- E*G) )/ M};
    if(beta<0.f || beta > 1.f - gamma) {return false;}

    const auto t{-1.f*(F*(A*K - J*B) + E*(J*C - A*L) + D*(B*L - K*C)) / M};

    if(t > high || t < low) {return false;} //If parameter is outside the range, ignore it
    high = t;
    return true;

}