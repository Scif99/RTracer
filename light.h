#pragma once
#include "vec3.h"

/*
A class for a point light source
*/

class Light
{
    Point3 m_point_; //
    Color m_intensity_; //an rgb intensity

public:
    ~Light() = default;
    Light() = delete;
    //

    constexpr Light(Point3 p, Color c)
        : m_point_{p}, m_intensity_{c} {}

    constexpr Point3 position() const noexcept { return m_point_;}
    constexpr Color intensity() const noexcept { return m_intensity_;}


};