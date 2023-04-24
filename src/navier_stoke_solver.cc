#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cmath>
#include "navier_stoke_solver.hh"

using std::pow;

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

using namespace nanogui;

void NavierStokeSolver::update(std::vector<Particle *>& grid_particles, double delta_t)
{
    if (grid_particles.size() == 0)
        return;
    for (Particle *p : grid_particles)
    {
        // 1 Compute the distances particle positions to the center
        // 2 Compute the density at each grid's position
        // ρᵢ = (315 M) / (64 π L⁹) ∑ⱼ (L² − dᵢⱼ²)³
        double sum = 0;
        for (Particle *q : grid_particles)
        {
            if (q == p)
                continue;
            nanogui::Vector3f diff_pos = p->pos - q->pos;
            double d = diff_pos.norm();
            sum += pow(p->L * p->L - d * d, 3);
        }
        p->density = std::max(sum * (315.0f * p->M) / (64.0f * M_PI * pow(p->L, 9.0f)), p->base_density);
        // p = κ * (ρ − ρ₀)
        p->pressure = std::max(p->fluid_stiffness * (p->density - p->base_density), (double)0.001f);
    }

    for (Particle* p : grid_particles)
    {
        // 4 Compute the pressure force of each particle
        // Pᵢ = (− (45 M) / (π L⁶)) ∑ⱼ [− (xⱼ − xᵢ) / (dᵢⱼ)] [(pⱼ + pᵢ) / (2 ρⱼ)] (L − dᵢⱼ)²
        // 5 Compute the viscosity force of each particle
        // Vᵢ = (45 μ M) / (π L⁶) ∑ⱼ (uⱼ − uᵢ) / (ρⱼ) (L − dᵢⱼ)
        nanogui::Vector3f pforce_vec = nanogui::Vector3f(0, 0, 0); // pressure forcs
        nanogui::Vector3f viscosity_vec = nanogui::Vector3f(0, 0, 0);
        for (Particle* q : grid_particles)
        {
            if (q == p)
                continue;
            nanogui::Vector3f diff_pos = p->pos - q->pos;
            double dist_ij = diff_pos.norm();
            pforce_vec += (diff_pos / dist_ij) * ((p->pressure + q->pressure) * pow((p->L - dist_ij), 2.0f) / (2.0f * q->density));
            viscosity_vec += (q->velocity - p->velocity) * (p->L - dist_ij) / q->density;
        }
        nanogui::Vector3f pressure_force = pforce_vec * (-45.0f * p->M) / (M_PI * pow(p->L, 6.0f));
        nanogui::Vector3f viscosity_force = viscosity_vec * 45.0f * p->M / (M_PI * pow(p->L, 6.0f));
        // 6 Add up the RHS
        // Fᵢ = Pᵢ + Vᵢ + G
        p->forces = pressure_force + viscosity_force + p->gravity;
    }
    for (Particle* p : grid_particles)
    {
        p->velocity += delta_t * p->forces / p->density;
        p->pos += delta_t * p->velocity;
        p->update(delta_t);
    }
}