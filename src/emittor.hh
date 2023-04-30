#include <nanogui/nanogui.h>
#include "particle.hh"

class Emittor
{

public:
    Emittor(){};

    Emittor(nanogui::Vector3f position, nanogui::Vector3f init_velocity, nanogui::Vector4f init_color)
    {
        this->position = position;
        this->init_velocity = init_velocity;
        this->init_color = init_color;
    }
    nanogui::Vector3f position;
    nanogui::Vector3f init_velocity;
    nanogui::Vector4f init_color;

    Particle emit(SmokeParameter *sp) const;
};