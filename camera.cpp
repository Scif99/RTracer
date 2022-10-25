#include "camera.h"
#include <numbers> // std::numbers


Camera::Camera(float vfov, float aspect_ratio)
{
    auto theta = vfov * std::numbers::pi_v<float> / 180.f;
    auto h = tan(theta/2);
    auto viewport_height = 2.0 * h;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    m_position_ = Point3(0, 0, 0);
    horizontal = Vec3(viewport_width, 0.0, 0.0);
    vertical = Vec3(0.0, viewport_height, 0.0);
    lower_left_corner = m_position_ - horizontal/2 - vertical/2 - Vec3(0, 0, focal_length);

}

Ray Camera::get_ray(double u, double v) const 
{
    return Ray(m_position_, lower_left_corner + u*horizontal + v*vertical - m_position_);
}