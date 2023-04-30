#include <random>
#include "emittor.hh"
#include "smoke_param.hh"

using namespace nanogui;

Particle Emittor::emit(SmokeParameter *sp) const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> angle_dis(0, 2 * M_PI);
    std::uniform_real_distribution<double> radius_dis(0, 5);
    std::uniform_real_distribution<double> height_dis(0, 5);
    std::uniform_real_distribution<double> vel_dis(5, 10);

    float angle = angle_dis(gen);
    float radius = radius_dis(gen);
    float height = height_dis(gen);
    Vector3f init_pos(radius * cos(angle), height - 5, radius * sin(angle));
    Vector3f rand_velocity(radius * cos(angle), vel_dis(gen) * 10, radius * sin(angle)); // Upward velocity
    Vector3f init_forces(radius * cos(angle), 0, radius * sin(angle));

    return Particle(init_pos, rand_velocity, init_forces, sp);
}
