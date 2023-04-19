#include <nanogui/nanogui.h>
#include "smoke_simulator.hh"

using namespace nanogui;

SmokeSimulator::SmokeSimulator(std::string shader_dir, nanogui::Screen *screen, GLFWwindow *window)
{
    shader = std::make_shared<nanogui::GLShader>();
    shader->initFromFiles("Default",
                          shader_dir + "/point.vert",
                          shader_dir + "/point.frag");

    Vector3f init_velocity = Vector3f(0, 0.5, 0);
    Vector4f init_color = Vector4f(66 / 256.0f, 135 / 256.0f, 245 / 256.0f, 1.0f);
    emittor = new Emittor(Vector3f(0, -1, 0), init_velocity, init_color);
    smoke = new Smoke();
    this->screen = screen;
    this->window = window;
}

void SmokeSimulator::draw()
{
    smoke->generateParticles(emittor, 100);
    smoke->update(1 / frames_per_sec);

    nanogui::MatrixXf positions(4, smoke->particles.size());
    nanogui::MatrixXf colors(4, smoke->particles.size());
    nanogui::MatrixXf sizes(1, smoke->particles.size());

    shader->bind();
    for (int i = 0; i < smoke->particles.size(); ++i)
    {
        positions.col(i) << smoke->particles[i].pos, 1;
        colors.col(i) << smoke->particles[i].color;
        sizes.col(i) << smoke->particles[i].size;
    }
    shader->uploadAttrib("in_position", positions, false);
    shader->uploadAttrib("in_color", colors, false);
    shader->uploadAttrib("in_size", sizes, false);
    shader->drawArray(GL_POINTS, 0, smoke->particles.size());
}

std::string strval = "A string";
Color colval(0.5f, 0.5f, 0.7f, 1.f);
void SmokeSimulator::initGUI()
{
    screen->initialize(window, true);
    std::cout << "Created screen" << std::endl;

    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Smoke Simulator Attributes");
    gui->addGroup("Basic types");
    gui->addVariable("string", strval);

    gui->addGroup("Color Picker");
    gui->addVariable("Color", colval)
        ->setFinalCallback([this](const Color &c)
                           { std::cout << "ColorPicker Final Callback: ["
                                       << c.r() << ", "
                                       << c.g() << ", "
                                       << c.b() << ", "
                                       << c.w() << "]" << std::endl; 
                                this->emittor->init_color = Vector4f(c.r(), c.g(), c.b(), c.w()); });

    screen->setVisible(true);
    screen->performLayout();

    // Modify later for more features
    // glfwSetCursorPosCallback(window, SmokeSimulator::cursorPosCallback);
    // glfwSetMouseButtonCallback(window, SmokeSimulator::mouseButtonCallback);
    // glfwSetKeyCallback(window, SmokeSimulator::keyCallback);
    // glfwSetCharCallback(window, SmokeSimulator::charCallback);
    // glfwSetDropCallback(window, SmokeSimulator::dropCallback);
    // glfwSetScrollCallback(window, SmokeSimulator::scrollCallback);
    // glfwSetFramebufferSizeCallback(window, SmokeSimulator::framebufferSizeCallback);
    std::cout << "GUI initialized" << std::endl;
};

// ----------------------------------------------------------------------------
// EVENT HANDLING
// ----------------------------------------------------------------------------
void SmokeSimulator::cursorPosCallbackEvent(double x, double y)
{
    screen->cursorPosCallbackEvent(x, y);
}
void SmokeSimulator::mouseButtonCallbackEvent(int button, int action, int modifiers)
{
    screen->mouseButtonCallbackEvent(button, action, modifiers);
}
void SmokeSimulator::keyCallbackEvent(int key, int scancode, int action, int mods)
{
    screen->keyCallbackEvent(key, scancode, action, mods);
}
void SmokeSimulator::charCallbackEvent(unsigned int codepoint)
{
    screen->charCallbackEvent(codepoint);
}
void SmokeSimulator::dropCallbackEvent(int count, const char **filenames)
{
    screen->dropCallbackEvent(count, filenames);
}
void SmokeSimulator::scrollCallbackEvent(double x, double y)
{
    screen->scrollCallbackEvent(x, y);
}
void SmokeSimulator::resizeCallbackEvent(int width, int height)
{
    screen->resizeCallbackEvent(width, height);
}