#pragma once
#include "vec3.h"

/*
A class for a point light source
*/

class Light
{
    Point3 m_point; //
    Color m_intensity; //the color of the light in rgb

public:
    ~Light() = default;
    Light() = delete;
    //

    constexpr Light(Point3 p, Color c)
        : m_point{p}, m_intensity{c} {}

    constexpr Point3 position() const noexcept { return m_point;}
    constexpr Color intensity() const noexcept { return m_intensity;}


};