#include "camera.h"
#include <numbers> // std::numbers


Camera::Camera(Point3 lookfrom, Point3 lookat, Vec3 vup,float vfov, float aspect_ratio)
{
    const auto theta = vfov * std::numbers::pi_v<float> / 180.f;
    const auto h{tanf(theta/2)};
    const auto viewport_height{2.f * h};
    const auto viewport_width {aspect_ratio * viewport_height};

    const auto w{ unit_vector(lookfrom - lookat) };
    const auto u{ unit_vector(cross(vup, w)) };
    const auto v{ cross(w, u) };

    m_position = lookfrom;
    horizontal = viewport_width * u;
    vertical = viewport_height * v;
    lower_left_corner = m_position - horizontal/2 - vertical/2 - w;

}

Ray Camera::get_ray(float s, float t) const {
    return Ray(m_position, lower_left_corner + s*horizontal + t*vertical - m_position);
}

//overload for moving objects (motion blur)
Ray Camera::get_ray(float s, float t, float time0, float time1) const {
    return Ray(m_position, lower_left_corner + s*horizontal + t*vertical - m_position, RNG::get().generate_exponentialfloat() /*RNG::get().generate_float(time0, time1)*/);
}