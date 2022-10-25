#pragma once

#include <cmath>

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>

#include "utility.h"


class Vec3
{
    std::array<float, 3> elem;
public:
    //Aliases
    using Point3 = Vec3;
    using Color = Vec3;

    ~Vec3() = default;
    constexpr Vec3() = default;
    constexpr Vec3(const Vec3& other) = default;
    constexpr Vec3& operator=(const Vec3& other) = default;
    constexpr Vec3(Vec3&& other) noexcept = default;
    constexpr Vec3& operator=(Vec3&& other) noexcept = default;

    constexpr Vec3(float f)
        : elem{f,f,f} {}
    
    explicit constexpr Vec3(float x, float y, float z)
        : elem{x,y,z} {}

    constexpr float x() const noexcept {return elem[0];}
    constexpr float y() const noexcept {return elem[1];}
    constexpr float z() const noexcept {return elem[2];}

    constexpr Vec3 operator-() const noexcept {return Vec3(-elem[0], -elem[1], -elem[2]);}
    constexpr float& operator[](int i) {return elem[i];}    
    constexpr const float& operator[](int i) const {return elem[i];}

    
    constexpr Vec3& operator+=(const Vec3& other) {elem[0]+=other[0];elem[1]+=other[1];elem[2]+=other[2]; return *this;}
    constexpr Vec3& operator*=(const float t)  {elem[0]*=t; elem[1]*= t;elem[2]*= t; return *this;}
    constexpr Vec3& operator/=(const float t)  {return *this *= 1/t;}


    constexpr float length_squared() const noexcept {return elem[0]*elem[0]+ elem[1]*elem[1]+ elem[2]*elem[2];};
    constexpr float length() const {return sqrtf(length_squared());}

    static Vec3 random() { return Vec3(RNG::get().generate_float(0.f,1.f), RNG::get().generate_float(0.f,1.f), RNG::get().generate_float(0.f,1.f));}

    static Vec3 random(float min, float max) {
        return Vec3(RNG::get().generate_float(min,max), RNG::get().generate_float(min,max), RNG::get().generate_float(min,max));
        }

    friend std::ostream& operator<<(std::ostream &out, const Vec3 &v);
    friend bool operator== (const Vec3& v1, const Vec3& v2);
    // constexpr friend Vec3 operator+(const Vec3& u, const Vec3& v);
    // constexpr friend Vec3 operator-(const Vec3& u, const Vec3& v);
    // constexpr friend Vec3 operator*(float f, const Vec3& vec);
    // constexpr friend Vec3 operator*(const Vec3& vec, float f); 
    // constexpr friend Vec3 operator/(const Vec3& vec, float f);
    // constexpr friend float dot(const Vec3& u, const Vec3& v);
    // constexpr friend Vec3 cross(const Vec3& u, const Vec3& v);
    // constexpr friend Vec3 unit_vector(const Vec3& vec);


};

//Aliases
using Point3 = Vec3;
using Color = Vec3;

inline bool operator == (const Vec3& v1, const Vec3& v2)
{
    return v1.elem == v2.elem;
}

inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) 
{
    return out << v.elem[0] << ' ' << v.elem[1] << ' ' << v.elem[2];
}

inline constexpr Vec3 operator+(const Vec3& u, const Vec3& v) 
{
    return Vec3{u[0]+ v[0], u[1]+ v[1],u[2]+ v[2]};
}

inline constexpr Vec3 operator-(const Vec3& u, const Vec3& v) 
{
    return Vec3{u[0]-v[0], u[1]-v[1], u[2]-v[2]};
}

inline constexpr Vec3 operator*(const Vec3 &u, const Vec3 &v) 
{
    return Vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline constexpr Vec3 operator*(float f, const Vec3& vec) 
{
    return Vec3{f*vec[0],f*vec[1],f*vec[2]};
}

inline constexpr Vec3 operator*(const Vec3& vec, float f) 
{
    return f*vec;
}


inline constexpr Vec3 operator/(const Vec3& vec, float t) noexcept
{
    return (1.f/t)*vec;
}

inline constexpr float dot(const Vec3& u, const Vec3& v)
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

inline constexpr Vec3 cross(const Vec3& u, const Vec3& v)
{
    return Vec3(u[1]*v[2]-u[2]*v[1],
                u[2]*v[0]-u[0]*v[2], 
                u[0]*v[1]-u[1]*v[0]);
}

inline constexpr Vec3 unit_vector(const Vec3& vec)
{
    return vec/vec.length();
}

// computes the reflected ray given an in direction and an *outward* normal
inline Vec3 reflected(const Vec3& d, const Vec3& n)
{
    //assert, d, n are normalised
    return d - 2*dot(d,n)*n;
}

//returns the direction of a refracted ray if refraction is possible.
// outward_normal should be the outwards-facing normal of the surface
inline std::optional<Vec3> refracted(const Vec3& d,const Vec3& n, float refraction_ratio)
{
    
    //asssert : in_direction and outward_normal are both normalised

    //---------------------------------


    // const auto cos_theta{ fminf( dot(-d, n), 1.f) };
    // const auto r_out_perp{ refraction_ratio * (d + cos_theta * n) };
    // const auto r_out_parallel{ -sqrtf(fabs(1.f - r_out_perp.length_squared())) * n };
    // const auto transmitted{ r_out_perp + r_out_parallel };

    //----------------------------------------------


    //compute component of transmitted ray parallel to surface
    const auto cos_theta{ fminf( dot(-d,n), 1.f)};
    const Vec3 out_parallel = refraction_ratio * (d + cos_theta * n);

    //compute component of transmitted ray perpendicular to surface
    float d_dot_n_sq = dot(d,n)*dot(d,n);
    
    float ratio_sq = refraction_ratio*refraction_ratio;

    float discriminant = 1 - (ratio_sq*(1 - d_dot_n_sq)) ;

    if(discriminant < 0 ) return std::nullopt ; //no refraction

    const Vec3 out_perp = -sqrtf(discriminant) * n; ///I FORGOT TO MULTIPLY  by n....
    const Vec3 transmitted = out_parallel + out_perp;

    return transmitted;
}



inline void print_color(std::ostream &out, const Color& pixel_color, int samples_per_pixel) {

    using std::clamp;

    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples with a gamma correction value of 2
    const auto scale = 1.f / samples_per_pixel;
    r = sqrtf(r*scale);
    g = sqrtf(g*scale);
    b = sqrtf(b*scale);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(g, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(b, 0.f, 0.999f)) << '\n';
}