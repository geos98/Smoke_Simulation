#include "emittor.hh"

using namespace nanogui;

Particle Emittor::emit() const
{
    double a = (((double)rand() / (double)RAND_MAX) - 0.5);
    double b = (((double)rand() / (double)RAND_MAX) - 0.5);
    double c = (((double)rand() / (double)RAND_MAX) - 0.5);
    Vector3f init_pos = Vector3f(a * 2, b * 10, c * 2);
    Vector3f rand_velocity = Vector3f(a * 50, 0, b * 50);
    Vector3f init_forces = Vector3f(a * 10, 0, 0);
    float init_size = 1;
    return Particle(init_pos, rand_velocity, init_forces, init_color, init_size);
}
