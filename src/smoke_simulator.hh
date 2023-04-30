
#include <nanogui/nanogui.h>
#include <string>
#include <vector>
#include <memory>

#include "smoke.hh"
#include "camera.hh"
#include "collision/plane.hh"

class SmokeSimulator
{
public:
    SmokeSimulator(std::string shader_dir, nanogui::Screen *screen, GLFWwindow *window);
    ~SmokeSimulator(){};

    Smoke smoke;
    Emittor emittor;
    GLFWwindow *window;

    float frames_per_sec = 10.0f;

    void draw();
    void initGUI();

    bool cursorPosCallbackEvent(double x, double y);
    bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    bool keyCallbackEvent(int key, int scancode, int action, int mods);
    bool charCallbackEvent(unsigned int codepoint);
    bool dropCallbackEvent(int count, const char **filenames);
    bool scrollCallbackEvent(double x, double y);
    bool resizeCallbackEvent(int width, int height);

    void loadCollisionObjects();

private:
    std::shared_ptr<nanogui::GLShader> shader;
    // std::vector<Particle> particles;

    // Camera attributes

    Camera camera;
    Camera canonicalCamera;

    double view_distance;
    double canonical_view_distance;
    double min_view_distance;
    double max_view_distance;

    double scroll_rate;

    // Screen methods

    nanogui::Screen *screen;
    void mouseLeftDragged(double x, double y);
    void mouseRightDragged(double x, double y);
    void mouseMoved(double x, double y);
    virtual void resetCamera();
    virtual nanogui::Matrix4f getProjectionMatrix();
    virtual nanogui::Matrix4f getViewMatrix();

    // Mouse flags

    bool left_down = false;
    bool right_down = false;
    bool middle_down = false;

    // Keyboard flags

    bool ctrl_down = false;

    // Simulation flags

    bool is_paused = true;

    // Screen attributes

    int mouse_x;
    int mouse_y;

    int screen_w;
    int screen_h;

    bool is_alive = true;

    nanogui::Vector2i default_window_size = nanogui::Vector2i(400, 400);

    vector<CollisionObject*>* collision_objects;
};