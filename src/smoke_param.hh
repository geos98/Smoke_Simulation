#ifndef SMOKE_PARAM_HH
#define SMOKE_PARAM_HH
#include <nanogui/nanogui.h>
#include <list>

using nanogui::Vector3f;
using nanogui::Vector4f;

struct Parameter
{
    double value;
    double lower;
    double upper;
    // double default_value;
    operator double() const
    {
        return value;
    }
};

class SmokeParameter
{

public:
    SmokeParameter(){};
    ~SmokeParameter(){};

    // ---------------------------------------------------------
    // Smoke parameters
    // ---------------------------------------------------------
    bool two_layers = false;                                                // whether uses two layers
    Parameter size = Parameter{1.0f, 1.0f, 10.0f};                          // particle size
    Parameter lifespan = Parameter{3.0f, 1.0f, 10.0f};                      // particle lifespan
    Parameter L = Parameter{1.5f, 1.0f, 3.0f};                              // smoothed particle length
    Parameter M = Parameter{0.01f, 0.01f, 5.0f};                            // particle mass
    Parameter damping = Parameter{2.0f, 0.0f, 10.0f};                       // damping coefficient
    Parameter vorticity_epsilon = Parameter{0.01f, 0.01f, 10.0f};           // vorticity epsilon
    Parameter ambient_temp = Parameter{27.0f, 0.0f, 100.0f};                // ambient temperature
    Parameter fluid_stiffness = Parameter{100.0f, 0.0f, 1000.0f};           // fluid stiffness
    Parameter buoyancy_coefficient = Parameter{1.0f, 0.0f, 10.0f};          // buoyancy coefficient
    Parameter thermal_expansion_coefficient = Parameter{0.5f, 0.0f, 10.0f}; // thermal expansion coefficient
    Parameter air_drag_coefficient = Parameter{0.1f, 0.0f, 10.0f};          // air drag coefficient
    Parameter dynamic_viscosity_coefficient = Parameter{0.1f, 0.0f, 10.0f}; // dynamic viscosity coefficient

    // ---------------------------------------------------------
    // Vector attributes
    // ---------------------------------------------------------
    Vector3f gravity = Vector3f(0.0f, -9.81f, 0.0f);                                           // gravity
    nanogui::Color init_color = nanogui::Color(50.0f / 256, 168.0f / 256, 128.0f / 256, 1.0f); // color
};
#endif