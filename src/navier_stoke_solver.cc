#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include "navier_stoke_solver.hh"

using std::pow;
using namespace nanogui;

// for calculating density
double cubic_spline_kernel(double r, double h)
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

// Smoothed Particle Hydrodynamics (SPH). Uses the super-simple
// approch of Matthias Müller: https://matthias-research.github.io/pages/publications/sca03.pdf
// -> New Particles enter the domain slightly below the top with a velocity
// directed to the bottom left (like pouring in the beer)
// ----------
// Solution Strategy:
// Discretize the fluid by N particles (i=0, 1, ..., N-1) that smooth
// their properties radially with some smoothing kernels. Then the Momentum
// Equation discretizes to

// Duᵢ/Dt = Pᵢ + Vᵢ + G
// with
// uᵢ : The velocity of the i-th smoothed particle
// Pᵢ : The pressure forces acting on the i-th smoothed particle
// Vᵢ : The viscosity forces acting on the i-th smoothed particle
// G  : The gravity forces (acting equally on all particles)

// This yields a set of N ODEs each for the two velocity components (in case
// of 2D) of the particles. These can be solved using a (simpletic) integrator
// to advance the position of the particles.
// Let xᵢ be the 2D position of each smoothed particle.
// Let L be the smoothing length of each smoothed particle.
// Let M be the mass of each smoothed particle.
// ------

// Algorithm
// (for details on the chosen smoothing kernels, see the paper mentioned above)
// 1. Compute the rhs for each particle Fᵢ
//     1.1 Compute the distances between all particle positions
//         dᵢⱼ = || xᵢ − xⱼ ||₂
//     1.2 Compute the density at each particle's position
//         ρᵢ = (315 M) / (64 π L⁹) ∑ⱼ (L² − dᵢⱼ²)³
//     1.3 Compute the pressure at each particle's position (κ is the isentropic
//         exponent, ρ₀ is a base density)
//         pᵢ = κ * (ρ − ρ₀)
//     1.4 Compute the pressure force of each particle
//         Pᵢ = (− (45 M) / (π L⁶)) ∑ⱼ − (xⱼ − xᵢ) / (dᵢⱼ) (pⱼ + pᵢ) / (2 ρⱼ) (L − dᵢⱼ)²
//     1.5 Compute the viscosity force of each particle
//         Vᵢ = (45 μ M) / (π L⁶) ∑ⱼ (uⱼ − uᵢ) / (ρⱼ) (L − dᵢⱼ)
//     1.6 Add up the RHS
//         Fᵢ = Pᵢ + Vᵢ + G

// 2. Integrate the Ordinary Differential Equation  "ρ Duᵢ/Dt = Fᵢ" with a
//    Δt timestep
//     2.1 Update the particles' velocities
//         uᵢ ← uᵢ + Δt Fᵢ / ρᵢ
//     2.2 Update the particles' positions
//         xᵢ ← xᵢ + Δt uᵢ

// void NavierStokeSolver::update(std::vector<Particle *> grid_particles, double delta_t)
// {
//     if (grid_particles.size() == 0)
//         return;
//     for (Particle *p : grid_particles)
//     {
//         // 1 Compute the distances particle positions to the center
//         // 2 Compute the density at each grid's position
//         // ρᵢ = (315 M) / (64 π L⁹) ∑ⱼ (L² − dᵢⱼ²)³
//         double sum = 0;
//         p->density = 0;
//         for (Particle *q : grid_particles)
//         {
//             if (q == p)
//                 continue;
//             nanogui::Vector3f diff_pos = p->pos - q->pos;
//             double d = diff_pos.norm();
//             sum += pow(p->L * p->L - d * d, 3);
//             p->density += q->M * cubic_spline_kernel(d, (double)1);
//         }
//         // p->density = std::max(sum * (315.0f * p->M) / (64.0f * M_PI * pow(p->L, 9.0f)), p->base_density);
//         // p = κ * (ρ − ρ₀)
//         p->pressure = std::max(p->fluid_stiffness * (p->density - p->base_density), (double)0.0001f);
//     }

//     for (Particle *p : grid_particles)
//     {
//         // 4 Compute the pressure force of each particle
//         // Pᵢ = (− (45 M) / (π L⁶)) ∑ⱼ [− (xⱼ − xᵢ) / (dᵢⱼ)] [(pⱼ + pᵢ) / (2 ρⱼ)] (L − dᵢⱼ)²
//         // 5 Compute the viscosity force of each particle
//         // Vᵢ = (45 μ M) / (π L⁶) ∑ⱼ (uⱼ − uᵢ) / (ρⱼ) (L − dᵢⱼ)
//         nanogui::Vector3f pforce_vec = nanogui::Vector3f(0, 0, 0); // pressure forcs
//         nanogui::Vector3f viscosity_vec = nanogui::Vector3f(0, 0, 0);
//         for (Particle *q : grid_particles)
//         {
//             if (q == p)
//                 continue;
//             nanogui::Vector3f diff_pos = p->pos - q->pos;
//             double dist_ij = diff_pos.norm();
//             pforce_vec += (diff_pos / dist_ij) * ((p->pressure + q->pressure) * pow((p->L - dist_ij), 2.0f) / (2.0f * q->density));
//             viscosity_vec += (q->velocity - p->velocity) * (p->L - dist_ij) / q->density;
//         }
//         nanogui::Vector3f pressure_force = pforce_vec * (-45.0f * p->M) / (M_PI * pow(p->L, 6.0f));
//         nanogui::Vector3f viscosity_force = viscosity_vec * 45.0f * p->M / (M_PI * pow(p->L, 6.0f));
//         std::cout << pressure_force << std::endl;
//         // 6 Add up the RHS
//         // Fᵢ = Pᵢ + Vᵢ + G
//         p->forces = pressure_force + viscosity_force + p->gravity;
//     }
//     for (Particle *p : grid_particles)
//     {
//         p->velocity += delta_t * p->forces / p->density;
//         p->pos += delta_t * p->velocity;
//         p->update(delta_t);
//     }
// }

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

void NavierStokeSolver::update_rho_p(std::vector<Particle *> grid_particles, Particle *avg_p, double delta_t)
{
    if (grid_particles.size() == 0)
        return;
    avg_p = new Particle();
    for (Particle *q : grid_particles)
    {
        avg_p->pos += q->pos;
        avg_p->velocity += q->velocity;
        avg_p->forces += q->forces;
    }
    avg_p->pos /= grid_particles.size();
    avg_p->velocity /= grid_particles.size();
    avg_p->forces /= grid_particles.size();

    for (Particle *q : grid_particles)
    {
        nanogui::Vector3f diff_pos = avg_p->pos - q->pos;
        double d = diff_pos.norm();
        double weight = cubic_spline_kernel(d, (double)q->L);
        avg_p->density += q->M * weight;
    }

    avg_p->pressure = avg_p->fluid_stiffness * avg_p->density;
}

void NavierStokeSolver::update_with_neighbour_cells(std::vector<Particle *> grid_particles, Particle *neighbour_p, double delta_t)
{
    if (grid_particles.size() == 0)
        return;

    nanogui::Vector3f pforce_vec = nanogui::Vector3f(0, 0, 0); // pressure forces
    nanogui::Vector3f viscosity_vec = nanogui::Vector3f(0, 0, 0);
    for (Particle *q : grid_particles)
    {
        // 4 Compute the pressure force of each particle
        // 5 Compute the viscosity force of each particle
        // pressure forces
        if (neighbour_p->density == 0 || q->density == 0)
            continue;
        double p_factor = -(neighbour_p->M * (neighbour_p->pressure / (neighbour_p->density * neighbour_p->density)) +
                            q->M * (q->pressure / (q->density * q->density)));
        nanogui::Vector3f kernel_gradient = cubic_spline_kernel_gradient(neighbour_p->pos - q->pos, q->L);
        pforce_vec += p_factor * kernel_gradient;

        // viscosity forces
        double mu = 0.1f; // dynamic viscosity coefficient
        nanogui::Vector3f v_factor = mu * q->M * (q->velocity - neighbour_p->velocity) / q->density;
        double kernel_laplacian = cubic_spline_kernel_laplacian(neighbour_p->pos - q->pos, q->L);
        viscosity_vec += v_factor * kernel_laplacian;

        std::cout << "v_fac: " << neighbour_p->pressure << " " << grid_particles.size() << std::endl;
    }

    // nanogui::Vector3f viscosity_force = viscosity_vec * 45.0f * p->M / (M_PI * pow(p->L, 6.0f));
    // 6 Add up the RHS Fᵢ = Pᵢ + Vᵢ + G
    p->forces = pforce_vec + viscosity_vec + p->gravity;

    for (Particle *q : grid_particles)
    {
        q->forces += p->forces;
        q->velocity += delta_t * q->forces / p->density;
        q->pos += delta_t * q->velocity;
        q->update(delta_t);
    }
}

void NavierStokeSolver::simplified_update(std::vector<Particle *> grid_particles, double delta_t)
{
    if (grid_particles.size() == 0)
        return;

    // Get average particle in a grid
    Particle *p = new Particle();
    for (Particle *q : grid_particles)
    {
        p->pos += q->pos;
        p->velocity += q->velocity;
        p->forces += q->forces;
    }
    p->pos /= grid_particles.size();
    p->velocity /= grid_particles.size();
    p->forces /= grid_particles.size();

    double total_weight = 0.0f; // for later distribute forces to neighbours
    for (Particle *q : grid_particles)
    {
        // 1 Compute the distances particle positions to the center
        // 2 Compute the density at each grid's position
        // ρᵢ = (315 M) / (64 π L⁹) ∑ⱼ (L² − dᵢⱼ²)³
        nanogui::Vector3f diff_pos = p->pos - q->pos;
        double d = diff_pos.norm();
        double weight = cubic_spline_kernel(d, (double)q->L);
        total_weight += weight;
        p->density += q->M * weight;
    }

    // p = κ * (ρ − ρ₀)
    p->pressure = p->fluid_stiffness * p->density;

    nanogui::Vector3f pforce_vec = nanogui::Vector3f(0, 0, 0); // pressure forces
    // nanogui::Vector3f pforce_vec = nanogui::Vector3f(0, 0, 0); // pressure forces
    nanogui::Vector3f viscosity_vec = nanogui::Vector3f(0, 0, 0);
    for (Particle *q : grid_particles)
    {
        // 4 Compute the pressure force of each particle
        // 5 Compute the viscosity force of each particle
        // pressure forces
        if (p->density == 0 || q->density == 0)
            continue;
        double p_factor = -(p->M * (p->pressure / (p->density * p->density)) +
                            q->M * (q->pressure / (q->density * q->density)));
        nanogui::Vector3f kernel_gradient = cubic_spline_kernel_gradient(p->pos - q->pos, q->L);
        pforce_vec += p_factor * kernel_gradient;

        // viscosity forces
        double mu = 0.1f; // dynamic viscosity coefficient
        nanogui::Vector3f v_factor = mu * q->M * (q->velocity - p->velocity) / q->density;
        double kernel_laplacian = cubic_spline_kernel_laplacian(p->pos - q->pos, q->L);
        viscosity_vec += v_factor * kernel_laplacian;

        // update density pressure for q
        std::cout << "v_fac: " << p->pressure << " " << grid_particles.size() << std::endl;
        q->density = p->density;
        q->pressure = p->pressure;
    }
    // std::cout << "p: " << viscosity_vec << " " << grid_particles.size() << std::endl;
    // nanogui::Vector3f viscosity_force = viscosity_vec * 45.0f * p->M / (M_PI * pow(p->L, 6.0f));
    // 6 Add up the RHS Fᵢ = Pᵢ + Vᵢ + G
    p->forces = pforce_vec + viscosity_vec + p->gravity;

    for (Particle *q : grid_particles)
    {
        double scale = cubic_spline_kernel((q->pos - p->pos).norm(), p->L) / total_weight;
        q->forces += p->forces * scale;
        q->velocity += delta_t * q->forces / p->density;
        q->pos += delta_t * q->velocity;
        q->update(delta_t);
    }
}
