#pragma once

#include <algorithm>

#include "vec3.h"
#include "rt_stb_image.h"

class Texture
{
public:
    virtual Color getColor(float u, float v) const =0;
    virtual Color getColor(const std::pair<float, float>& texture_coord) const =0;

};


class SolidColor : public Texture
{
private:
    Color m_color;
public:
    SolidColor(float r, float g, float b)
        : m_color{r,g,b} {}

    SolidColor(const Color& color)
        : m_color{color} {}

    Color getColor(float u, float v) const override{return m_color;}
    Color getColor(const std::pair<float, float>& texture_coord) const override{return m_color;}

};

class ImageTexture : public Texture
{
private:
    unsigned char *data;
    int width, height;
    int bytes_per_scanline;

public:
    const static int bytes_per_pixel = 3;

public:
    ~ImageTexture() {delete data;}
    ImageTexture()
        : data{nullptr}, width{0}, height{0}, bytes_per_scanline{0} {}

    ImageTexture(const char* filename) 
    {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);

        //image could not be loaded
        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    Color getColor(float u, float v) const override;
    Color getColor(const std::pair<float, float>& texture_coord) const override{return getColor(texture_coord.first, texture_coord.second);}

};

Color ImageTexture::getColor(float u, float v) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (data == nullptr)
        return Color(0,1,1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = std::clamp(u, 0.f, 1.f);
    v = 1.f - std::clamp(v, 0.f, 1.f);  // Flip V to image coordinates

    auto i = static_cast<int>(u * width);
    auto j = static_cast<int>(v * height);

    // Clamp integer mapping, since actual coordinates should be less than 1.0
    if (i >= width)  i = width-1;
    if (j >= height) j = height-1;

    const auto color_scale = 1.0 / 255.0;
    auto pixel = data + j*bytes_per_scanline + i*bytes_per_pixel;

    return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
}
