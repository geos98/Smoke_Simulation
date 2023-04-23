#include "emittor.hh"

using namespace nanogui;

Particle Emittor::emit()
{
    double a = (((double)rand() / (double)RAND_MAX) - 0.5);
    double b = (((double)rand() / (double)RAND_MAX) - 0.5) * 0.1;
    double c = (((double)rand() / (double)RAND_MAX) - 0.5);
    Vector3f rand_velocity = Vector3f(init_velocity[0] + a, init_velocity[1] + b, init_velocity[2] + c);
    Vector3f init_forces = Vector3f(-0.01f, -0.1f, -0.01f);
    float init_size = 1;
    return Particle(position, rand_velocity, init_forces, init_color, init_size);
}
