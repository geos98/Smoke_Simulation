#ifndef NAVIER_STOKE_SOLVER_HH
#define NAVIER_STOKE_SOLVER_HH
#include "particle.hh"
#include <nanogui/nanogui.h>

// Smoothed Particle Hydrodynamics (SPH). Uses the super-simple
// approch of Matthias Müller: https://matthias-research.github.io/pages/publications/sca03.pdf

// struct NavierStokesProperty
// {
//     double viscosity{0.0001f};
//     double pressure{0.0f};
//     double density{1.0f};
//     nanogui::Vector3f velocity{0.0f, 0.0f, 0.0f};
//     nanogui::Vector3f forces{0.0f, 0.0f, 0.0f};
// };

class NavierStokeSolver
{
    // Compressible Model
    // ρ Du/Dt = − ∇p + μ ∇²u + g
    // u : velocity
    // ρ : density
    // p : pressure
    // μ : dynamics viscosity
    // g : gravity
    // Du/Dt : Lagrangian temporal derivative (=Material Derivative)
    // ∇p    : Pressure Gradient
    // ∇²u   : Velocity Laplacian (=collection of second derivatives)
public:
    NavierStokeSolver()
    {
        // this->viscosity = 0.0001f;
        // this->pressure = 0.0f;
        // this->density = 1.0f;
        // this->velocity = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
        // this->forces = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
        this->base_density = 0.001f;
        this->fluid_stiffness = 0.01f;
    };
    // double viscosity;
    // double pressure;
    // double density;
    // nanogui::Vector3f center;
    // nanogui::Vector3f velocity;
    // nanogui::Vector3f forces;
    ~NavierStokeSolver(){};

    // TODO: maybe change this to adjustable
    double base_density;
    double fluid_stiffness;
    nanogui::Vector3f gravity = nanogui::Vector3f(0.0f, -9.81f, 0.0f);

    void update(const std::vector<Particle *>& grid_particles, double delta_t);
};

#endif