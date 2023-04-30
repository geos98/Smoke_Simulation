#include <iostream>
#include <random>
#include <vector>

#include "particle.hh"
#include "smoke_param.hh"

using namespace nanogui;

void Particle::update(double delta_t)
{
    buoyancy = -(sp->buoyancy_coefficient * (temperature - sp->ambient_temp) + sp->thermal_expansion_coefficient * density) * sp->gravity;
    vorticity = vorticity_new;
    forces += buoyancy + (-sp->damping.value * velocity * vorticity.norm());
    velocity += delta_t * forces;
    pos += delta_t * velocity;

    // update color & lifespan
    lifespan -= delta_t * ((double)rand() / (double)RAND_MAX);
    Vector4f alpha = Vector4f(0, 0, 0, 1 / (1 + std::exp(27.0f - temperature)));
    color += alpha;
}

nanogui::Vector3f compute_buoyancy_force(double density_diff, double temperature_diff, double alpha, double beta, nanogui::Vector3f gravity)
{

    // The buoyancy force is directed upwards (in the positive y direction)
    nanogui::Vector3f buoyancy_force = -(alpha * temperature_diff + beta * density_diff) * gravity;
    return buoyancy_force;
}