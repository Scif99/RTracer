#ifndef BVH_H
#define BVH_H

#include <memory>
#include <iostream>
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "ray.h"


class BVHNode : public Hittable {
public:

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& list, int low, int high)
        : box(Vec3(0.f,0.f,0.f),Vec3(1.f,1.f,1.f)) 
    {
           
        assert(!(list.empty()));
        auto obj = list;

        //#0 Base cases
        if(low + 1 ==high) {
            left = list[low];
            right = nullptr;
        }

        else if(low+2==high) {
            left = std::min(obj[low],obj[low+1], [](auto&& h1, auto&& h2) {return h1->BoundingBox().min.X() < h2->BoundingBox().min.X();});
            right = std::max(obj[low],obj[low+1], [](auto&& h1, auto&& h2) {return h2->BoundingBox().min.X() > h2->BoundingBox().min.X();});
        }

        else {
            //#1 sort along some random axis
            std::sort(obj.begin() + low , obj.begin() + high, [](auto&& h1, auto&& h2) {
                return h2->BoundingBox().min.X() < h1->BoundingBox().min.X();
            });

            //#2 Split
            const int mid = low + (high - low)/2;
            left = std::make_shared<BVHNode>(obj,low,mid);
            right = std::make_shared<BVHNode>(obj,mid,high);
        }
        
        //Note that both children shouldnt be null since the input is not empty
        if(!left) box = right->BoundingBox();
        if(!right) box = left->BoundingBox();
        else box = SurroundingBox(left->BoundingBox(), right->BoundingBox());

    }


    explicit BVHNode(const HittableList& h) 
        : BVHNode(h.m_objects, 0, h.m_objects.size())
        {}

    //Recursively traverse the tree
    [[nodiscard]] std::optional<HitData> Hit(const Ray& ray, float t_low, float t_high) const override {
        //If the ray doesnt intersect the enclosing volume at this node, then it will not hit any primitives in the subtree. Return early.
        if(!box.Intersects(ray,t_low,t_high)) return std::nullopt;

        //Return data from closest collision
        const auto left_data = left ? left->Hit(ray,t_low,t_high) : std::nullopt;
        const auto right_data = right ?  right->Hit(ray,t_low,t_high) : std::nullopt;
        if(!left_data) return right_data;
        if(!right_data) return left_data;
        return std::min(left_data, right_data, [](auto&& a, auto&& b) {return b.value().hit_param > a.value().hit_param;});
        
        //TODO 
    }

    [[nodiscard]] AABB BoundingBox() const override {return box;}
private:
    AABB box; //AABB that encloses all primitives in the subtree 
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
};

#endif
