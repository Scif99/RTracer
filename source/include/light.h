#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"


/// @brief Point light source
struct PointLight {
    
Point3 position; 
Color intensity; //the color of the light in rgb
};

#endif