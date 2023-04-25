#ifndef PARTICLE_HH
#define PARTICLE_HH
#include <nanogui/nanogui.h>

class Particle
{

public:
    Particle(){};
    Particle(nanogui::Vector3f pos, nanogui::Vector3f velocity, nanogui::Vector3f forces, nanogui::Vector4f color, float size)
    {
        this->pos = pos;
        this->velocity = velocity;
        this->forces = forces;
        this->color = color;
        this->size = size;
        this->lifespan = 3.0f;
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
    double density = 0.001f;
    double pressure = 1.0f;
    double base_density = 0.001f;
    double fluid_stiffness = 0.01f;
    nanogui::Vector3f gravity = nanogui::Vector3f(0.0f, -0.00981f, 0.0f);
    void update(double delta_t);
};

#endif