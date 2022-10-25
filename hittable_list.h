#pragma once

#include "hittable.h"

#include <memory>
#include <optional>
#include <vector>

class HittableList : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> m_objects;

public:
    ~HittableList() = default;
    HittableList() = default;

    void add(std::shared_ptr<Hittable> object) { m_objects.push_back(object); }

    std::optional<HitData> hit(const Ray& ray, float t_low, float t_high) const override;
};

//Function that returns data from the closest collision between a ray and the scene objects, if a collision occured.
std::optional<HitData> HittableList::hit(const Ray& ray, float t_low, float t_high) const
{
    std::optional<HitData> data;
    auto closest_so_far{ t_high }; //store the parameter of the closest intersection so far
    for(const auto& object : m_objects)
    {
        if(const auto tmp_data = object->hit(ray, t_low, closest_so_far); tmp_data)
        {
            closest_so_far = tmp_data.value().hit_param; //update the max range for t if ray hits something
            data = tmp_data.value(); //update data of closest intersection
        }
    }
    return data;
}