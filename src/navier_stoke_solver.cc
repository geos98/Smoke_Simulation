#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include "navier_stoke_solver.hh"

using std::pow;
using namespace nanogui;

// for calculating density
double cubic_spline_kernel(double r /* distance */, double h)
{
    double q = r / h;

    double coefficient = 1.0 / (M_PI * std::pow(h, 3));

    if (q >= 0 && q < 1)
    {
        return coefficient * (1 - (3.0 / 2.0) * q * q + (3.0 / 4.0) * q * q * q);
    }
    else if (q >= 1 && q < 2)
    {
        return coefficient * (1.0 / 4.0) * std::pow((2 - q), 3);
    }
    else
    {
        return 0;
    }
}

nanogui::Vector3f cubic_spline_kernel_gradient(nanogui::Vector3f dxyz, double h)
{
    double r = dxyz.norm();
    double q = r / h;

    double coefficient = 1.0 / (M_PI * std::pow(h, 4));
    double dW;

    if (q >= 0 && q < 1)
    {
        dW = coefficient * (-3.0 * q + 9.0 / 4.0 * q * q);
    }
    else if (q >= 1 && q < 2)
    {
        dW = coefficient * (-3.0 / 4.0 * std::pow((2 - q), 2));
    }
    else
    {
        dW = 0;
    }

    nanogui::Vector3f gradient = dW * dxyz / r;
    return gradient;
}
double cubic_spline_kernel_laplacian(nanogui::Vector3f dxyz, double h)
{
    double r = dxyz.norm();
    double q = r / h;

    double coefficient = 1.0 / (M_PI * std::pow(h, 5));
    double d2W;

    if (q >= 0 && q < 1)
    {
        d2W = coefficient * (12.0 / 4.0 * q - 6.0);
    }
    else if (q >= 1 && q < 2)
    {
        d2W = coefficient * (-3.0 * std::pow((2 - q), 1));
    }
    else
    {
        d2W = 0;
    }

    return d2W;
}

double update_particle_temperature(Particle *p, Particle *neighbor, double delta_t, double W_grad)
{
    double diffusion_coeff = 0.024f;
    double dT = std::max(p->temperature - neighbor->temperature, (double)0.001f);
    return delta_t * diffusion_coeff * dT * W_grad;
}

nanogui::Vector3f compute_vorticity(Particle *p, Particle *neighbor, double W_grad)
{
    nanogui::Vector3f vorticity = nanogui::Vector3f(0, 0, 0);
    nanogui::Vector3f d = neighbor->pos - p->pos;
    // Compute the velocity difference between the particles
    nanogui::Vector3f dv = neighbor->velocity - p->velocity;

    // Calculate the contribution to the vorticity from this neighbor
    vorticity[0] += (dv[1] * d[2] - dv[2] * d[1]) * W_grad;
    vorticity[1] += (dv[2] * d[0] - dv[0] * d[2]) * W_grad;
    vorticity[2] += (dv[0] * d[1] - dv[1] * d[0]) * W_grad;
    return vorticity;
}

nanogui::Vector3f compute_vorticity_confinement_force(Particle *p, Particle *neighbor, double W_grad)
{
    nanogui::Vector3f vorticity_confine = nanogui::Vector3f(0, 0, 0);
    // double epsilon = 0.01f;
    double epsilon = p->sp->vorticity_epsilon;

    // Compute the vorticity difference between the particles
    double dvorticity_magnitude = (neighbor->vorticity - p->vorticity).norm();

    // Calculate the contribution to the gradient of the vorticity magnitude from this neighbor
    nanogui::Vector3f grad_vorticity_magnitude = dvorticity_magnitude * (neighbor->pos - p->pos) * W_grad;

    vorticity_confine[0] = epsilon * p->L() * (grad_vorticity_magnitude[1] * p->vorticity[2] - grad_vorticity_magnitude[2] * p->vorticity[1]);
    vorticity_confine[1] = epsilon * p->L() * (grad_vorticity_magnitude[2] * p->vorticity[0] - grad_vorticity_magnitude[0] * p->vorticity[2]);
    vorticity_confine[2] = epsilon * p->L() * (grad_vorticity_magnitude[0] * p->vorticity[1] - grad_vorticity_magnitude[1] * p->vorticity[0]);
    return vorticity_confine;
}

void NavierStokeSolver::update_avg_p(std::vector<Particle *> grid_particles, Particle *avg_p)
{
    if (grid_particles.size() == 0)
        return;

    for (Particle *q : grid_particles)
    {
        avg_p->pos += q->pos;
        avg_p->velocity += q->velocity;
        avg_p->forces += q->forces;
        avg_p->vorticity += q->vorticity;
        avg_p->buoyancy += q->buoyancy;
        avg_p->temperature += q->temperature;
    }
    avg_p->pos /= grid_particles.size();
    avg_p->velocity /= grid_particles.size();
    avg_p->forces /= grid_particles.size();
    avg_p->vorticity /= grid_particles.size();
    avg_p->buoyancy /= grid_particles.size();
    avg_p->temperature /= grid_particles.size();

    for (Particle *q : grid_particles)
    {
        avg_p->density += q->sp->M * cubic_spline_kernel((avg_p->pos - q->pos).norm(), (double)q->L());
    }
    avg_p->pressure = avg_p->sp->fluid_stiffness * avg_p->density;

    // reset and align all particles in the grid with the avg particle
    for (Particle *p : grid_particles)
    {
        p->buoyancy = nanogui::Vector3f(0, 0, 0);
        p->vorticity_new = nanogui::Vector3f(0, 0, 0);
        p->density = avg_p->density;
        p->pressure = avg_p->pressure;
        p->forces = avg_p->sp->gravity;
        p->temperature = avg_p->temperature;
    }
}

nanogui::Vector3f compute_air_drag_force(Particle *p)
{

    // double air_drag_coefficient = 0.01f;
    // Calculate the air drag force
    nanogui::Vector3f air_drag_force = -p->sp->air_drag_coefficient * p->velocity;
    return air_drag_force;
}
nanogui::Vector3f compute_turbulence_force(Particle *p)
{
    double turbulence_strength = 0.1f;
    // Generate a random vector
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);

    nanogui::Vector3f random_vector(dis(gen), dis(gen), dis(gen));

    // Calculate the turbulence force
    nanogui::Vector3f turbulence_force = turbulence_strength * random_vector;

    return turbulence_force;
}
nanogui::Vector3f compute_damping_force(Particle *p)
{
    // Damping force based on particle height
    return p->velocity * (-0.2f * std::max(p->pos[1], 0.0f));
}

void NavierStokeSolver::update_with_neighbour_cells(std::vector<Particle *> grid_particles, Particle *neighbour_p, double delta_t)
{
    if (grid_particles.size() == 0)
        return;

    for (Particle *q : grid_particles)
    {
        // Calculate the kernel gradient for the current distance
        nanogui::Vector3f d = neighbour_p->pos - q->pos;
        double distance = d.norm();
        double W = std::max(cubic_spline_kernel(distance, q->L()), (double)0.0001f);
        double W_grad = std::max((double)cubic_spline_kernel_gradient(d, q->L()).norm(), (double)0.0001f);

        // temperature
        q->temperature += update_particle_temperature(neighbour_p, q, delta_t, W);

        // buoyancy forces
        // q->buoyancy += compute_buoyancy_force(q->density - neighbour_p->density, q->temperature - q->ambient_temp, buoyancy_coefficient, thermal_expansion_coefficient, q->gravity);
        // q->forces += q->buoyancy;

        // vorticity confinement forces
        q->forces += compute_vorticity_confinement_force(neighbour_p, q, W_grad);
        // pressure forces
        double p_factor = -(neighbour_p->sp->M * (neighbour_p->pressure / (neighbour_p->density * neighbour_p->density)) +
                            q->sp->M * (q->pressure / (q->density * q->density)));
        nanogui::Vector3f kernel_gradient = cubic_spline_kernel_gradient(neighbour_p->pos - q->pos, q->L());
        q->forces += p_factor * kernel_gradient;

        // viscosity forces
        nanogui::Vector3f v_factor = q->sp->dynamic_viscosity_coefficient * q->sp->M * (q->velocity - neighbour_p->velocity) / q->density;
        double kernel_laplacian = cubic_spline_kernel_laplacian(neighbour_p->pos - q->pos, q->L());
        q->forces += v_factor * kernel_laplacian;

        // air drag forces
        q->forces += compute_air_drag_force(q);

        // compute new vorticity, but update later
        q->vorticity_new += compute_vorticity(q, neighbour_p, W_grad);
    }
}
