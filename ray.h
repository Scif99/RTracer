#pragma once

#include "vec3.h"

/*
- A ray is defined by an origin and a direction vector
- The Ray constructor normalises the direcction vector parameter

- Note: the direction vector is NOT automatically normalised within the constructor.
This is so that we can perform instancing correctly
*/

class Ray
{
private:
    Vec3 m_dir;
    Point3 m_origin;
    float m_time;
public:

    ~Ray() = default;
    constexpr Ray() = delete;
    constexpr Ray(const Ray&) = default;
    constexpr Ray& operator=(const Ray&) = default;
    constexpr Ray(Ray&&) = default; 
    constexpr Ray& operator=(Ray&&) = default;

    
    constexpr Ray(const Point3& origin, const Vec3& dir, float time=0.f)
        : m_dir{dir}, m_origin{origin}, m_time{time} {} //should direction be normalised???

    constexpr Point3 at(float t) const noexcept {return m_origin + t * m_dir;}

    constexpr Point3 origin() const noexcept {return m_origin;}
    constexpr Vec3 direction() const noexcept {return m_dir;}
    constexpr float time() const noexcept { return m_time;}
};