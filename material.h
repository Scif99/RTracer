#pragma once

#include <memory>

#include "texture.h"
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
        : m_type{type}, m_albedo{std::make_shared<SolidColor>(color)} {}

    Material(MaterialType type, std::shared_ptr<Texture> texture)
        : m_type{type}, m_albedo{texture} {}
    
public:
    MaterialType m_type;
    std::shared_ptr<Texture> m_albedo;

};
