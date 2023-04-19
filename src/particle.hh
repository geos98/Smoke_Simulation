#include <nanogui/nanogui.h>

using namespace std;
using namespace nanogui;

class Particle
{

public:
    Particle(Vector3f pos, Vector3f velocity, Vector3f forces, Vector4f color, float size)
    {
        this->pos = pos;
        this->velocity = velocity;
        this->forces = forces;
        this->color = color;
        this->size = size;
        this->lifespan = 3.0f;
    };
    ~Particle(){};
    Vector3f pos;
    Vector3f velocity;
    Vector3f forces;
    Vector4f color; // R, G, B, alpha
    float size;
    float lifespan;

    void update(double delta_t);
};