#include <nanogui/nanogui.h>
#include "particle.hh"

using namespace nanogui;
using namespace std;

class Emittor
{

public:
    Emittor(Vector3f position, Vector3f init_velocity, Vector4f init_color)
    {
        this->position = position;
        this->init_velocity = init_velocity;
        this->init_color = init_color;
    }
    Vector3f position;
    Vector3f init_velocity;
    Vector4f init_color;

    Particle emit();
};