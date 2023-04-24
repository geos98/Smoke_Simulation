#include <iostream>
#include <random>
#include <vector>

#include "particle.hh"

using namespace nanogui;

void Particle::update(double delta_t)
{
    double a = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    double b = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    double c = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    // pos += Vector3f(velocity[0] + a, velocity[1] + b, velocity[2] + c) * delta_t;
    float sum = pos[0] + pos[1] + pos[2];
    // velocity += forces * delta_t;
    lifespan -= delta_t * ((double)rand() / (double)RAND_MAX);
    color -= Vector4f(pos[0] / sum, pos[1] / sum, pos[2] / sum, ((double)rand() / (double)RAND_MAX));
}