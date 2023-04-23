#include <nanogui/nanogui.h>

class Particle
{

public:
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

    void update(double delta_t);
};