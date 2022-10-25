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
    data.mat_ptr = m_mat_ptr;
    return data;
}
