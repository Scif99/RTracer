#pragma once

#include <cmath>

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <random>


class Vec3
{
public:
    constexpr Vec3()
        : elem{0.f,0.f,0.f} {}
    
    constexpr Vec3(float f)
        : elem{f,f,f} {}
    
    constexpr Vec3(float x, float y, float z)
        : elem{x,y,z} {}


    //Default copy operations
    constexpr Vec3(const Vec3& other) = default;
    constexpr Vec3& operator=(const Vec3& other) = default;
    
    //Default move operations
    constexpr Vec3(Vec3&& other) = default;
    constexpr Vec3& operator=(Vec3&& other) = default;

    constexpr float x() const noexcept {return elem[0];}
    constexpr float y() const noexcept {return elem[1];}
    constexpr float z() const noexcept {return elem[2];}


    constexpr float& operator[](int i) {return elem[i];} //noexcept?
    constexpr const float& operator[](int i) const {return elem[i];}

    constexpr Vec3 operator-() const noexcept {return Vec3(-elem[0], -elem[1], -elem[2]);}
    constexpr Vec3& operator+=(const Vec3& other)  noexcept {elem[0]+=other[0];elem[1]+=other[1];elem[2]+=other[2]; return *this;}

    constexpr float length_squared() const {return (elem[0]*elem[0])+(elem[1]*elem[1])+(elem[2]*elem[2]);};
    constexpr float length() const {return sqrt(length_squared());} //noexcept?
    


    friend std::ostream& operator<<(std::ostream &out, const Vec3 &v);

private:
    std::array<float, 3> elem;
};

std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.elem[0] << ' ' << v.elem[1] << ' ' << v.elem[2];
}

constexpr Vec3 operator+(const Vec3& u, const Vec3& v) noexcept
{
    return Vec3{u[0]+v[0],u[1]+v[1],u[2]+v[2]};
}

constexpr Vec3 operator-(const Vec3& u, const Vec3& v) noexcept
{
    return Vec3{u[0]-v[0],u[1]-v[1],u[2]-v[2]};
}


constexpr Vec3 operator*(float f, const Vec3& vec) noexcept
{
    return Vec3{f*vec[0],f*vec[1],f*vec[2]};
}

constexpr Vec3 operator*(const Vec3& vec, float f) noexcept
{
    return f*vec;
}

constexpr Vec3 operator/(const Vec3& vec, float f) noexcept
{
    return Vec3(vec[0]/f,vec[1]/f,vec[2]/f);
}


constexpr float dot(const Vec3& u, const Vec3& v)
{
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

constexpr Vec3 cross(const Vec3& u, const Vec3& v)
{
    return Vec3(u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0]);
}

constexpr Vec3 unit_vector(const Vec3& vec)
{
    return vec/vec.length();
}

//Aliases
using Point3 = Vec3;
using Color = Vec3;

void print_color(std::ostream &out, const Color& pixel_color, int samples_per_pixel) {

    using std::clamp;

    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.f / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(g, 0.f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(b, 0.f, 0.999f)) << '\n';
}