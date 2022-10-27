#include <cmath>
#include <numbers>

#include "sphere.h"

/*
- The hit function determines whether a given ray intersects the sphere
- Since we only care about the closest hittable, we also pass in a low and high parameter
*/

std::optional<HitData> Sphere::hit(const Ray& r, float t_low, float t_high) const
{
    //First we sub the parametric equation for a ray into the parametric equation for a circle.
    //The resulting equation is a quadratic in the parameter t. Thus a root exists iff t has two distinct roots.

    //computer coefficients of the quadratic
    const auto oc{r.origin() - m_centre};
    const auto A{r.direction().length_squared()};
    const auto half_B{dot(r.direction(), oc)}; 
    const auto C{dot(oc,oc) - m_radius*m_radius};
    const auto discriminant = half_B*half_B - A*C;

    if(discriminant < 0 ) return std::nullopt; //no roots

    const auto sqrt_disc = sqrtf(discriminant);
    //Use discriminant to compute smaller value of t at closest intersection point
    float t = (-half_B - sqrt_disc)/A;

    //this root is outside the range, try other root
    if(t < t_low || t_high < t ) 
    {
        //this case occurs when the ray is inside a sphere, e.g. during refraction
        t = (-half_B + sqrt_disc)/A;
        if(t < t_low|| t_high < t) 
        {
            return std::nullopt;
        }
    }

    HitData data;
    data.hit_param = t;
    data.hit_point = r.at(t);
    data.hit_normal = (data.hit_point - m_centre) / m_radius;
    data.tex_coords = get_texture_coords(data.hit_normal); //NOTE WE USED THE NORMAL NOT THE HIT POINT!
    data.mat_ptr = m_mat_ptr;
    return data;
}   

std::pair<float, float> Sphere::get_texture_coords(const Point3& p)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    const auto theta = acos(-p.y());
    const auto phi = atan2(-p.z(), p.x()) + std::numbers::pi_v<float>;

    float u =  phi / (2*std::numbers::pi_v<float>);
    float v = theta / std::numbers::pi_v<float>;

    return std::make_pair(u,v);
}


/*
- the only difference is that we must use the centre at the correct time
*/

std::optional<HitData> MovingSphere::hit(const Ray& r, float t_low, float t_high) const
{
    //First we sub the parametric equation for a ray into the parametric equation for a circle.
    //The resulting equation is a quadratic in the parameter t. Thus a root exists iff t has two distinct roots.

    //computer coefficients of the quadratic
    const auto oc{r.origin() - centre(r.time())};
    const auto A{r.direction().length_squared()};
    const auto half_B{dot(r.direction(), oc)}; 
    const auto C{dot(oc,oc) - m_radius*m_radius};
    const auto discriminant = half_B*half_B - A*C;

    if(discriminant < 0 ) return std::nullopt; //no roots

    const auto sqrt_disc = sqrtf(discriminant);
    //Use discriminant to compute smaller value of t at closest intersection point
    float t = (-half_B - sqrt_disc)/A;

    //this root is outside the range, try other root
    if(t < t_low || t_high < t ) 
    {
        //this case occurs when the ray is inside a sphere, e.g. during refraction
        t = (-half_B + sqrt_disc)/A;
        if(t < t_low|| t_high < t) 
        {
            return std::nullopt;
        }
    }

    HitData data;
    data.hit_param = t;
    data.hit_point = r.at(t);
    data.hit_normal = (data.hit_point - centre(r.time())) / m_radius;
    data.tex_coords = get_texture_coords(data.hit_normal); //NOTE WE USED THE NORMAL NOT THE HIT POINT!
    data.mat_ptr = m_mat_ptr;
    return data;
}


std::pair<float, float> MovingSphere::get_texture_coords(const Point3& p)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    const auto theta = acos(-p.y());
    const auto phi = atan2(-p.z(), p.x()) + std::numbers::pi_v<float>;

    float u = phi / (2*std::numbers::pi_v<float>);
    float v = theta / std::numbers::pi_v<float>;
    return std::make_pair(u,v);
}