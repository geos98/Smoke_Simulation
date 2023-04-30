#include "emittor.hh"
#include "smoke_param.hh"

using namespace nanogui;

Particle Emittor::emit(SmokeParameter *sp)
{
    auto angle = angle_dis(gen);
    auto radius = radius_dis(gen);
    auto height = height_dis(gen);
    Vector3f init_pos(radius * cos(angle), height - 5.0, radius * sin(angle));
    Vector3f rand_velocity(radius * cos(angle), vel_dis(gen) * 10.0, radius * sin(angle)); // Upward velocity
    Vector3f init_forces(radius * cos(angle), 0.0, radius * sin(angle));

    return Particle(init_pos, rand_velocity, init_forces, sp);
}
