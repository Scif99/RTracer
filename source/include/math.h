#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <optional>
#include <utility>
#include <iterator>
#include <vector>

#include "vec3.h"

/// @brief Returns the root(s) of a quadratic equation ax^2 + bx + c = 0, if they exist, or null if they don't.
/// @param a Quadratic coefficient
/// @param b Linear Coefficient
/// @param c Constant coefficient
/// @return Optional pair containing the roots of the quadratic in increasing order
inline std::optional<std::pair<float,float>> SolveQuadratic(float a, float b, float c) {
    const auto discr{b*b - 4.f*a*c};
    if(discr < 0.f) return std::nullopt;
    if(discr == 0) {
        auto r{-0.5f*b/a}; 
        return std::make_pair(r,r);
    }
    auto q = (b > 0) ?
            -0.5 * (b + sqrt(discr)) :
            -0.5 * (b - sqrt(discr));
    
        auto x0{q / a};
        auto x1{c / q};
        if (x0 > x1) {std::swap(x0, x1);}
        return std::make_pair(x0,x1);
};


/// @brief Linearly interpolates between two points in 3d
/// @param a Initial point
/// @param b Final point
/// @param mix_value 
/// @return Lerp'd value
inline Vec3 Mix(const Vec3& a, const Vec3& b, float mix_value) {
    return a*(1.f-mix_value) + b*mix_value;
};


// computes the reflected ray given an in direction and an *outward* normal
/// @brief Computes a reflected ray about a surface normal.
/// @param I Incident vector
inline Vec3 Reflected(const Vec3& I, const Norm3& N)
{
    //assert, d, n are normalised
    return I - 2.f * Dot(I,N) * N;
}


/// @brief Note that the ray might be.
/// @brief Note this assumes the 'outer' material is air with a refractive index of 1.
/// @param I Unit direction of incident ray.
/// @param N Outwards normal of the surface.
/// @param eta Refractive index of material.
/// @return The 
inline std::optional<Norm3> Refracted(const Norm3& I, Norm3 N, float eta)
{
    //If the incident ray is inside the sphere
    if(Dot(N,I) > 0.f) {
        return Refracted(I,Norm3(-N),1.f/eta);
    }

    const auto angle = Dot(N,I);
    assert(angle<=0.f);

    const auto ratio = 1.f/eta;
    const auto ratio_sq = ratio*ratio;

    const auto determinant = 1.f - ratio_sq*(1.f-(angle*angle));
    if(determinant<0) return std::nullopt; //TIR

    const auto perp = ratio * (I - N*angle);
    const auto par =  - N * sqrtf(determinant);

    return Norm3(perp+par);
}


/// @brief Returns reflectance, the proportion of light that is reflected
inline float Fresnel(const Vec3& I, const Norm3& N, float eta) {
    auto cosi{Dot(I,N)};
    auto etai =1.f, etat = eta;
    if(cosi>0) {
        std::swap(etai,etat);
    }
    float sint = etai/etat * sqrtf(std::max(0.f, 1 - cosi * cosi));

    // Total internal reflection
    if (sint >= 1) {
        return 1;
    }

    else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2;
    }
}

#endif