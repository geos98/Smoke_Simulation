#include "particle.hh"

void Particle::update(double delta_t)
{
    double a = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    double b = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    double c = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.05;
    pos += Vector3f(velocity[0] + a, velocity[1] + b, velocity[2] + c) * delta_t;
    velocity += forces * delta_t;
    lifespan -= delta_t * ((double)rand() / (double)RAND_MAX);
    color -= Vector4f(0.0f, 0.0f, 0.0f, delta_t * ((double)rand() / (double)RAND_MAX));
}