#pragma once
#include "vec3.h"

class Ray
{
public:
    constexpr Ray()  {};
    constexpr Ray(const Point3& origin, const Vec3& dir)
        : m_dir_{dir}, m_origin_{origin} {}

    constexpr Point3 at(float t) const noexcept {return m_origin_ + t*m_dir_;}

    constexpr Point3 origin() const noexcept {return m_origin_;}
    constexpr Vec3 direction() const noexcept {return m_dir_;}

private:
    Vec3 m_dir_;
    Point3 m_origin_;
};