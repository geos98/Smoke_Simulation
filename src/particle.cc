#include <iostream>
#include <random>
#include <vector>

#include "particle.hh"

using namespace nanogui;

void Particle::update(double delta_t)
{
    buoyancy = -(buoyancy_coefficient * (temperature - ambient_temp) + thermal_expansion_coefficient * density) * gravity;
    vorticity = vorticity_new;

    forces += buoyancy;
    velocity += delta_t * forces;
    pos += delta_t * velocity;
    float sum = velocity[0] + velocity[1] + velocity[2];
    // velocity += forces * delta_t;
    lifespan -= delta_t * ((double)rand() / (double)RAND_MAX);
    color -= Vector4f(velocity[0] / sum, velocity[1] / sum, velocity[2] / sum, lifespan / 30.0f);
}