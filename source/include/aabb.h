#ifndef AABB_H
#define AABB_H

#include "hittable.h"
#include <array>
#include <algorithm>

//Returns intersection of intervals (x0,x1) and (y0,y1)
inline std::optional<std::array<float,2>> Overlap(float x0, float x1, float y0, float y1) {
    assert(x0<=x1 && y0<=y1);

    if(y1 < x0 || y0 > x1) return std::nullopt;

    std::array<float,2> interval;
    
    interval[0] = std::max(x0,y0);
    interval[1] = std::min(x1,y1);
    return interval;
}

class AABB : public Hittable{

public:
    constexpr AABB(const Vec3& vmin, const Vec3& vmax)
        : min{vmin}, max{vmax} { for(int i =0;i<2;++i) {assert(min[i] < max[i]);}}

    [[nodiscard]] std::optional<HitData> Hit(const Ray& ray, float t_low, float t_high) const override {


        //#1 Get t intervals in each dimension
        //X planes
        float t_xmin = (min.X() - ray.Origin().X()) / ray.Direction().X();  //t where ray intersects xmin plane
        float t_xmax = (max.X() - ray.Origin().X()) / ray.Direction().X(); 
        if(t_xmin > t_xmax) {std::swap(t_xmin,t_xmax);}

        //Y planes
        float t_ymin = (min.Y() - ray.Origin().Y()) / ray.Direction().Y(); 
        float t_ymax = (max.Y() - ray.Origin().Y()) / ray.Direction().Y(); 
        if(t_ymin > t_ymax) {std::swap(t_ymin,t_ymax);}       
        //Z planes (remember camera faces -z so need to flip max/min)
        float t_zmin = (min.Z() - ray.Origin().Z()) / ray.Direction().Z(); 
        float t_zmax = (max.Z() - ray.Origin().Z()) / ray.Direction().Z(); 
        if(t_zmin > t_zmax) {std::swap(t_zmin,t_zmax);}

        //#2 Compute intersection of intervals 
        //Interval from SIDE VIEW 
        const auto yz_interval = Overlap(t_ymin,t_ymax,t_zmin,t_zmax);
        if(!yz_interval) return std::nullopt; //no overlap

        //Interval from SIDE VIEW
        const auto xyz_interval = Overlap(t_xmin,t_xmax, t_zmin, t_zmax);
        if(!xyz_interval) return std::nullopt;

        //Store information from the intersection
        HitData data = {
            0.f,
            Vec3(0.f,0.f,0.f),
            Norm3(Vec3(0.f,0.f,1.f)),
            nullptr
        };
        return data;
    }

private:
    Vec3 min;
    Vec3 max;
};

#endif