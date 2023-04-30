#ifndef PARTICLE_HH
#define PARTICLE_HH
#include <nanogui/nanogui.h>
#include "smoke_param.hh"

using nanogui::Vector2f;
using nanogui::Vector3f;
using nanogui::Vector4f;

class Particle
{

public:
    Particle(SmokeParameter *smoke_param)
    {
        this->pos = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
        this->velocity = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
        this->forces = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
        this->sp = smoke_param;
        this->color = smoke_param->init_color;
        this->lifespan = smoke_param->lifespan;
    };
    Particle(
        Vector3f pos,
        Vector3f velocity,
        Vector3f forces,
        SmokeParameter *smoke_param)
    {
        this->pos = pos;
        this->velocity = velocity;
        this->forces = forces;
        this->sp = smoke_param;
        this->color = smoke_param->init_color;
        this->lifespan = smoke_param->lifespan;
    };
    ~Particle(){};

    // ---------------------------------------------------------
    // Smoke parameters
    // ---------------------------------------------------------
    SmokeParameter *sp; // pointer to smoke parameters

    // ---------------------------------------------------------
    // Particle attributes
    // ---------------------------------------------------------
    double density = 0.0001f;                            // particle density
    double pressure = 0.0001f;                           // particle pressure
    double temperature = 27.0f;                          // particle temperature
    double lifespan = 3.0f;                              // particle lifespan
    Vector3f pos;                                        // position
    Vector3f velocity;                                   // velocity
    Vector3f forces;                                     // forces on the particle
    Vector4f color;                                      // R, G, B, alpha
    Vector2f uv = Vector2f(0.5, 0.5);                    // uv coordinates
    Vector3f buoyancy = Vector3f(0.0f, 0.0f, 0.0f);      // buoyancy force
    Vector3f vorticity = Vector3f(0.0f, 0.0f, 0.0f);     // vorticity before confining
    Vector3f vorticity_new = Vector3f(0.0f, 0.0f, 0.0f); // vorticity after confining
    // Vector3f vorticity_confine = Vector3f(0.0f, 0.0f, 0.0f); // vorticity confinement force

    // ---------------------------------------------------------
    // Methods
    // ---------------------------------------------------------
    void update(double delta_t);
    double L() const { return sp->L * sp->size; }; // calculate the particle length
};

#endif
