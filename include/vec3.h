#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>

#include "rng.h"


class Vec3
{
protected:
    std::array<float, 3> elem;
public:
    //Aliases
    using Point3 = Vec3;
    using Color = Vec3;

    constexpr ~Vec3() = default;
    constexpr Vec3() = default;
    constexpr Vec3(const Vec3& other) = default;
    constexpr Vec3& operator=(const Vec3& other) = default;
    constexpr Vec3(Vec3&& other) noexcept = default;
    constexpr Vec3& operator=(Vec3&& other) noexcept = default;

    explicit constexpr Vec3(float x, float y, float z)
        : elem{x,y,z} {}

    explicit constexpr Vec3(float x)
        : elem{x,x,x} {}
    constexpr float X() const noexcept {return elem[0];}
    constexpr float Y() const noexcept {return elem[1];}
    constexpr float Z() const noexcept {return elem[2];}

    constexpr Vec3 operator-() const noexcept {return Vec3(-elem[0], -elem[1], -elem[2]);}
    constexpr float& operator[](int i) {return elem[i];}    
    constexpr const float& operator[](int i) const {return elem[i];}

    
    constexpr Vec3& operator+=(const Vec3& other) {elem[0]+=other[0];elem[1]+=other[1];elem[2]+=other[2]; return *this;}
    constexpr Vec3& operator*=(const float t)  {elem[0]*=t; elem[1]*= t;elem[2]*= t; return *this;}
    constexpr Vec3& operator/=(const float t)  {return *this *= 1/t;}


    constexpr float LengthSquared() const noexcept {return elem[0]*elem[0]+ elem[1]*elem[1]+ elem[2]*elem[2];};
    constexpr float Length() const {return sqrtf(LengthSquared());}

    static Vec3 Random() { return Vec3(RNG::Get().GenerateFloat(0.f,1.f), RNG::Get().GenerateFloat(0.f,1.f), RNG::Get().GenerateFloat(0.f,1.f));}

    static Vec3 Random(float min, float max) {
        return Vec3(RNG::Get().GenerateFloat(min,max), RNG::Get().GenerateFloat(min,max), RNG::Get().GenerateFloat(min,max));
        }

    constexpr auto& Data() const noexcept{return elem;}

};





//Aliases
using Point3 = Vec3;
using Color = Vec3;

inline bool operator == (const Vec3& v1, const Vec3& v2)
{
    return (v1[0] == v2[0]) && (v1[1] == v2[1]) && (v1[2] == v2[2]);
}

inline std::ostream& operator<<(std::ostream &out, const Vec3 &v) 
{
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
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
    assert(t!=0);
    return (1.f/t)*vec;
}

inline constexpr Vec3 UnitVector(const Vec3& vec)
{
    assert(vec.Length()!=0); 
    return vec/vec.Length();
}

inline constexpr float Dot(const Vec3& u, const Vec3& v)
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

inline constexpr Vec3 Cross(const Vec3& u, const Vec3& v)
{
    return Vec3(u[1]*v[2]-u[2]*v[1],
                u[2]*v[0]-u[0]*v[2], 
                u[0]*v[1]-u[1]*v[0]);
}


//returns the direction of a refracted ray if refraction is possible.
// outward_normal should be the outwards-facing normal of the surface



/// @brief A norm3 is just a vector that is guaranteed to be of unit length.
/// @brief Using the type system like this allows us to make guarantees about the data. 
class Norm3 : public Vec3 {
public:

     /// @brief Constructs a normalised vec3 from a regular vec3
     explicit constexpr Norm3(const Vec3& vec )
        : Vec3::Vec3(UnitVector(vec)) {}//{assert(this->LengthSquared() == 1);} //TO-DO Do we need this assertion?
};



/// @brief Scales the color of a pixel to fit the file format, and prints it to a buffer
/// @param out Out buffer
/// @param pixel_color 
/// @param samples_per_pixel The number of samples (rays) taken per pixel
inline void PrintColor(std::ostream &out, const Color& pixel_color, int samples) {

    using std::clamp;

    auto r = pixel_color.X();
    auto g = pixel_color.Y();
    auto b = pixel_color.Z();

    //Each sample ray can add an amount of color between 0 and 1, so we need to divide by the number of samples to scale color to [0,1]
    // Divide the color by the number of samples with a gamma correction value of 2
    const auto scale = 1.f / samples;
    r = sqrtf(r*scale);
    g = sqrtf(g*scale);
    b = sqrtf(b*scale);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(g, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(b, 0.f, 0.999f)) << '\n';
}

#endif