#ifndef PARTICLE_HH
#define PARTICLE_HH
#include <nanogui/nanogui.h>

class Particle
{

public:
    Particle()
    {
        this->size = 1;
        this->lifespan = 3.0f;
        this->L = size * 1.5f;
        this->M = 0.1f;
    };
    Particle(nanogui::Vector3f pos, nanogui::Vector3f velocity, nanogui::Vector3f forces, nanogui::Vector4f color, float size)
    {
        this->pos = pos;
        this->velocity = velocity;
        this->forces = forces;
        this->color = color;
        this->size = size;
        this->lifespan = 3.0f;
        this->L = size * 1.5f;
        this->M = 0.1f;
    };
    ~Particle(){};

    nanogui::Vector3f pos;
    nanogui::Vector3f velocity;
    nanogui::Vector3f forces;
    nanogui::Vector4f color; // R, G, B, alpha

    float size;
    float lifespan;

    // Navier Stoke
    double L; // smoothed particle length
    double M; // particle mass
    double density = 0.0001f;
    double base_density = 0.0001f;
    double pressure = 0.0001f;
    double temperature = 27.0f;
    double ambient_temp = 27.0f;
    double fluid_stiffness = 100.0f;
    nanogui::Vector3f buoyancy = nanogui::Vector3f(0.0f, -0.001f, 0.0f);
    nanogui::Vector3f vorticity = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
    nanogui::Vector3f vorticity_confine = nanogui::Vector3f(0.0f, 0.0f, 0.0f);
    nanogui::Vector3f gravity = nanogui::Vector3f(0.0f, -0.981f, 0.0f);
    void update(double delta_t);
};

#endif
