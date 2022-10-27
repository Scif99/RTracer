#pragma once

#include <memory>
#include <optional>
#include <utility>

#include "hittable.h"
#include "vec3.h"

class Sphere : public Hittable
{
    float m_radius;
    Point3 m_centre;
    std::shared_ptr<Material> m_mat_ptr;

public:

    //Special members
    ~Sphere() = default;
    Sphere() = delete;
    Sphere(const Sphere&) = default;
    Sphere& operator=(const Sphere&) = default;
    Sphere(Sphere&&) noexcept = default;
    Sphere& operator=(Sphere&&) noexcept = default;

    //Constructor
    Sphere(const Vec3& centre, float radius, std::shared_ptr<Material> mat)
        :m_centre{centre}, m_radius{radius},  m_mat_ptr{mat} {} //{ assert(m_radius_>0);}

    constexpr Vec3 centre() const noexcept {return m_centre;}
    constexpr float radius() const noexcept {return m_radius;}

    virtual std::optional<HitData> hit(const Ray& r, float t_low, float t_high) const override;
    
    static std::pair<float, float> get_texture_coords(const Point3& p);
};

//-------------------
// Moving sphere for simulating motion blur
//--------------------

class MovingSphere : public Hittable
{
    float m_radius;
    Point3 m_centre0, m_centre1;
    float m_time0, m_time1;
    std::shared_ptr<Material> m_mat_ptr;

public:

    //Special members
    ~MovingSphere() = default;
    MovingSphere() = delete;
    MovingSphere(const MovingSphere&) = default;
    MovingSphere& operator=(const MovingSphere&) = default;
    MovingSphere(MovingSphere&&) noexcept = default;
    MovingSphere& operator=(MovingSphere&&) noexcept = default;

    //Constructor
    MovingSphere(const Vec3& centre0, const Vec3& centre1, float time0, float time1, float radius, std::shared_ptr<Material> mat)
        :m_centre0{centre0},m_centre1{centre1}, m_time0{time0}, m_time1{time1},  m_radius{radius},  m_mat_ptr{mat} {} //{ assert(m_radius_>0);}

    constexpr Vec3 centre(float time) const noexcept {return m_centre0 + ((time-m_time0)/m_time1-m_time0) * (m_centre1-m_centre0);} //linearly interpolate over time
    constexpr float radius() const noexcept {return m_radius;}

    virtual std::optional<HitData> hit(const Ray& r, float t_low, float t_high) const override;

    static std::pair<float, float> get_texture_coords(const Point3& p);


};