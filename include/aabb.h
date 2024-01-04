#ifndef AABB_H
#define AABB_H

#include <array>
#include <algorithm>

#include "ray.h"

//Optionally returns intersection of intervals (x0,x1) and (y0,y1)
static inline std::optional<std::array<float,2>> Overlap(float x0, float x1, float y0, float y1) {
    assert(!(x0>x1) && !(y0>y1));

    if(y1 < x0 || y0 > x1) return std::nullopt;
    if(x0==x1 || y0==y1) return std::nullopt;
    return std::array{std::max(x0,y0),std::min(x1,y1)}; 
}

class AABB {

public:
    constexpr AABB(const Vec3& vmin, const Vec3& vmax)
        : min{vmin}, max{vmax} { for(int i =0;i<2;++i) {assert(min[i] < max[i]);}}

    [[nodiscard]] bool Intersects(const Ray& ray, float t_low, float t_high) const {


        //#1 Get t intervals in each dimension
        //Note the ray might intersect the max plane before min plane... (e.g. if ray is moving in -ve x direction) 
        //We swap the order if this is the case.

        //X planes
        const auto rdx{1.f/ray.Direction().X()};
        float t_xmin = (min.X() - ray.Origin().X()) * rdx;  
        float t_xmax = (max.X() - ray.Origin().X()) * rdx; 
        if(t_xmin > t_xmax) {std::swap(t_xmin,t_xmax);} 

        //Y planes
        const auto rdy{1.f/ray.Direction().Y()};
        float t_ymin = (min.Y() - ray.Origin().Y()) * rdy; 
        float t_ymax = (max.Y() - ray.Origin().Y()) * rdy; 
        if(t_ymin > t_ymax) {std::swap(t_ymin,t_ymax);}    

        //Z planes (remember camera faces -z so need to flip max/min)
        const auto rdz{1.f/ray.Direction().Z()};
        float t_zmin = (min.Z() - ray.Origin().Z()) * rdz; 
        float t_zmax = (max.Z() - ray.Origin().Z()) * rdz; 
        if(t_zmin > t_zmax) {std::swap(t_zmin,t_zmax);}

        //#2 Compute intersection of intervals 
        //Interval from SIDE VIEW 
        const auto yz_interval = Overlap(t_ymin,t_ymax,t_zmin,t_zmax);
        if(!yz_interval) return false;

        //Interval from SIDE VIEW
        const auto xz_interval = Overlap(t_xmin,t_xmax, t_zmin, t_zmax);
        if(!xz_interval) return false;

        //Now we know that the ray intersects the AABB somewhere, but it might be outside the range specified by the caller
        //If you think of this range as another set of intersections, then we can compute another overlap to determine
        //Whether the intersection lies in the valid range.
        const auto xyz_interval = Overlap(xz_interval.value()[0],xz_interval.value()[1],
                                          yz_interval.value()[0],yz_interval.value()[1]);
        if(!xyz_interval) return false;

        return true;
    }

public:
    Vec3 min;
    Vec3 max;
};

[[nodiscard]] inline AABB SurroundingBox(const AABB& bb1, const AABB& bb2) {

        Vec3 min( std::min(bb1.min.X(),bb2.min.X()),
                std::min(bb1.min.Y(),bb2.min.Y()),
                std::min(bb1.min.Z(),bb2.min.Z()));

        Vec3 max( std::max(bb1.max.X(),bb2.max.X()),
                std::max(bb1.max.Y(),bb2.max.Y()),
                std::max(bb1.max.Z(),bb2.max.Z()));
    
        return AABB(min,max);
    };


#endif