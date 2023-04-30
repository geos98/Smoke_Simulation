#ifndef EMITTOR_H
#define EMITTOR_H
#include <random>
#include <nanogui/nanogui.h>
#include "particle.hh"

class Emittor
{
    std::mt19937 gen;
    std::uniform_real_distribution<double> angle_dis;
    std::uniform_real_distribution<double> radius_dis;
    std::uniform_real_distribution<double> height_dis;
    std::uniform_real_distribution<double> vel_dis;
    
public:
    
    Emittor():gen(std::random_device{}()){};

    Emittor(nanogui::Vector3f position, nanogui::Vector3f init_velocity, nanogui::Vector4f init_color) :
        gen(std::random_device{}()),
        angle_dis(0, 2 * M_PI),
        radius_dis(0, 5),
        height_dis(0, 5),
        vel_dis(5, 10)
    {
        this->position = position;
        this->init_velocity = init_velocity;
        this->init_color = init_color;
    }
    
    nanogui::Vector3f position;
    nanogui::Vector3f init_velocity;
    nanogui::Vector4f init_color;

    Particle emit(SmokeParameter *sp);
};
#endif