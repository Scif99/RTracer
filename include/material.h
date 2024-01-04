#ifndef MATERIAL_H
#define MATERIAL_H

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

    constexpr Material(MaterialType type, const Color& diffuse_comp=Color{0.8f,0.8f,0.8f}, const Color& spec_comp=Color{0.2f,0.2f,0.2f},  float spec_exp=25)
        : m_type{type}, Kd{diffuse_comp}, Ks{spec_comp}, specular_exponent{spec_exp} {}
    
public:
    MaterialType m_type;
    Color Kd; //diffuse component
    Color Ks; //specular component
    float specular_exponent; //used in blinn-phong illumination for specular component
};

#endif