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
        : m_type_{type}, m_albedo_{color} {}
    
public:
    MaterialType m_type_;
    Color m_albedo_;

};
