#include <nanogui/nanogui.h>
#include <string>
#include <vector>
#include <memory>

#include "particle.hh"

class SmokeSimulator {
public:
    SmokeSimulator(std::string shader_dir, nanogui::Screen *screen);
    ~SmokeSimulator() {};

    void draw();

private:
    std::shared_ptr<nanogui::GLShader> shader;
    std::vector<Particle> particles;
};