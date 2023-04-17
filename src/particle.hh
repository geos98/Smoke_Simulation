#include <nanogui/nanogui.h>

class Particle {

public:
    nanogui::Vector3f pos;
    void update(double delta_t);
};