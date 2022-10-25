#pragma once

#include "vec3.h"

class Material
{
public:
    enum class MaterialType 
    {
        DIFFUSE,
        MIRROR,
        DIELECTRIC
    };

    Material(MaterialType type, const Color& color)
        : m_type{type}, m_albedo{color} {}
    
public:
    MaterialType m_type;
    Color m_albedo;

};
