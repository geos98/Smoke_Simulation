
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
    Emittor *emittor;
    nanogui::Screen *screen;
    GLFWwindow *window;

    float frames_per_sec = 60.0f;

    void draw();
    void initGUI();
    Vector2i default_window_size = Vector2i(1024, 800);
    void cursorPosCallbackEvent(double x, double y);
    void mouseButtonCallbackEvent(int button, int action, int modifiers);
    void keyCallbackEvent(int key, int scancode, int action, int mods);
    void charCallbackEvent(unsigned int codepoint);
    void dropCallbackEvent(int count, const char **filenames);
    void scrollCallbackEvent(double x, double y);
    void resizeCallbackEvent(int width, int height);

private:
    std::shared_ptr<nanogui::GLShader> shader;
    std::vector<Particle> particles;
};