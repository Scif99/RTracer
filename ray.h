#pragma once
#include "vec3.h"

/*
- A ray is defined by an origin and a direction vector
- The Ray constructor normalises the direcction vector parameter

*/

class Ray
{

    Vec3 m_dir_;
    Point3 m_origin_;
public:

    ~Ray() = default;

    constexpr Ray() = delete;
    constexpr Ray(const Ray&) = default;
    constexpr Ray& operator=(const Ray&) = default;
    constexpr Ray(Ray&&) noexcept = default; 
    constexpr Ray& operator=(Ray&&) noexcept = default;


    constexpr Ray(const Point3& origin, const Vec3& dir)
        : m_dir_{unit_vector(dir)}, m_origin_{origin} {}

    constexpr Point3 at(float t) const noexcept {return m_origin_ + t*m_dir_;}

    constexpr Point3 origin() const noexcept {return m_origin_;}
    constexpr Vec3 direction() const noexcept {return m_dir_;}
};