#include "emittor.hh"

using namespace nanogui;

Particle Emittor::emit() const
{
    double a = (((double)rand() / (double)RAND_MAX) - 0.5);
    double b = (((double)rand() / (double)RAND_MAX) - 0.5);
    double c = (((double)rand() / (double)RAND_MAX) - 0.5);
    Vector3f init_pos = Vector3f(a / 100, b / 1000, c / 100);
    Vector3f rand_velocity = Vector3f(a, b, c);
    Vector3f init_forces = Vector3f(0, 0, 0);
    float init_size = 1;
    return Particle(init_pos, rand_velocity, init_forces, init_color, init_size);
}
