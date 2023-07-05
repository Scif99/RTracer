#include <cmath>
#include <numbers>

#include "math.h"
#include "sphere.h"

/// @brief Returns data from a ray-sphere intersection.
/// @brief We only care about the closest intersection, so we specify a range of values that the ray parameter must lie in to be considered. 
/// @param r Incoming ray object
/// @param t_low Minimum value of ray parameter considered
/// @param t_high Max value of ray parameter considered
/// @return An optional which contains data from the intersection, if one occured, or null.
std::optional<HitData> Sphere::Hit(const Ray& r, float t_low, float t_high) const
{
    //if(const auto hitaabb = m_aabb.Hit(r,t_low,t_high); !hitaabb) {return std::nullopt;}

    //The maths for an intersection between a ray and sphere results in a quadratic in the ray parameter t.
    //There is an intersection iff t has two distinct roots.
    const auto origin_to_centre_vec{r.Origin() - m_centre};
    const auto roots = SolveQuadratic(Dot(r.Direction(),r.Direction()), 
                                2 * Dot(r.Direction(), origin_to_centre_vec), 
                                Dot(origin_to_centre_vec,origin_to_centre_vec) - (m_radius * m_radius) );
    
    if(!roots) return std::nullopt; 
    
    const auto& [root_low, root_high] = roots.value();
    assert(root_low<=root_high);

    auto closest_root = root_low;
    //The smaller value of t corresponds to the closest intersection of the sphere
    //This root might however be outside of the range we are considering.
    if((root_low<t_low) || (root_low > t_high)){
        closest_root = root_high; 
        if((root_high<t_low) || (root_high > t_high)) {
            return std::nullopt;
        }
    }

    //Store information from the intersection
    HitData data = {
        closest_root,
        r.At(closest_root),
        Norm3(r.At(closest_root) - m_centre),
        m_mat_ptr
    };
    return  data;
}   

