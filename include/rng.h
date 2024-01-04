#ifndef UTILITY_H
#define UTILITY_H

#include <random>
/*
TODO: I think right now it will always generate the same seed...
*/

class RNG
{
    RNG() = default;
    //inline static std::random_device rd; //Is there a way to incorporate this?
    inline static std::mt19937 eng;

public:

    ~RNG() = default;
    RNG(const RNG& other) = delete;
    RNG& operator=(const RNG& other) = delete;

    static RNG& Get()
    {
        static RNG s_instance; //Instantiate on first use
        return s_instance;
    }
    float GenerateFloat(float low, float high) {std::uniform_real_distribution<float> dist(low,high); return dist(eng);}
    float GenerateExponentialFloat() {std::exponential_distribution<float> dist(0.5f); return dist(eng);}
};


/*

Below is the general process 

std::random_device rd; //generate a seed
std::mt19937 engine; //Set up an engine
engine.seed(rd); 
std::uniform_real_distribution<> dist(0.f,1.f); //create a distribution
dist(engine); //use the engine to generate the numbers!

*/



#endif