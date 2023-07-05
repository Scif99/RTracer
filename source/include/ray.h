#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/// @brief A ray is defined by an origin and a direction vector.
/// @brief Note that the direction vector is NO normalised on construction.
class Ray
{
private:
    Point3 m_origin;
    Vec3 m_dir;
public:

    constexpr Ray(const Point3& origin, const Vec3& dir)
        : m_origin{origin}, m_dir{dir} {}

    [[nodiscard]] constexpr Point3 At(float t) const noexcept {return m_origin + t * m_dir;}
    [[nodiscard]] constexpr Point3 Origin() const noexcept {return m_origin;}
    [[nodiscard]] constexpr Vec3 Direction() const noexcept {return m_dir;}
};

#endif