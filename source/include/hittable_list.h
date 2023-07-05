#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <memory>
#include <optional>
#include <vector>

#include "hittable.h"

/// @brief Represents a group of hittable objects
class HittableList : public Hittable
{
public:
    std::vector<std::shared_ptr<Hittable>> m_objects;
public:

    /// @brief Add to the list of hittables
    /// @param object 
    void Add(std::shared_ptr<Hittable> object) { m_objects.push_back(std::move(object)); }

    /// @brief Optionally returns data from the closest collision between a ray and a set of objects.
    [[nodiscard]] std::optional<HitData> Hit(const Ray& ray, float t_low, float t_high) const override {
        std::optional<HitData> data;
        auto closest_so_far{ t_high }; //store the parameter of the closest intersection so far

        //Iterate over all objects in the list. If we hit one that is closer than any other we previously hit, we update the data
        for(const auto& object : m_objects)
        {
            if(const auto tmp_data = object->Hit(ray, t_low, closest_so_far); tmp_data)
            {
                closest_so_far = tmp_data.value().hit_param; //Only care about intersections closer than any previous one
                data = tmp_data.value();
            }
        }
        return data;
    }
};

#endif