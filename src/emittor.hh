#include <nanogui/nanogui.h>
#include "particle.hh"

using namespace nanogui;
using namespace std;

class Emittor
{

public:
    Emittor(Vector3f position, Vector3f init_velocity, double speed)
    {
        this->position = position;
        this->speed = speed;
        this->init_velocity = init_velocity;
    }
    Vector3f position;
    Vector3f init_velocity;
    double speed;

    Particle emit();
};