
#include <nanogui/nanogui.h>
#include <string>
#include <vector>
#include <memory>

#include "smoke.hh"

class SmokeSimulator
{
public:
    SmokeSimulator(std::string shader_dir, nanogui::Screen *screen, GLFWwindow *window);
    ~SmokeSimulator(){};

    Smoke *smoke;
    nanogui::Screen *screen;
    GLFWwindow *window;

    int frames_per_sec = 60;

    void draw();
    void initGUI();
    Vector2i default_window_size = Vector2i(1024, 800);

private:
    std::shared_ptr<nanogui::GLShader> shader;
    std::vector<Particle> particles;
};