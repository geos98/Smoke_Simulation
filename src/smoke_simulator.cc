#include <nanogui/nanogui.h>
#include <random>

#include "smoke_simulator.hh"
#include "misc/camera_info.h"
#include "misc/misc.h"

using namespace std;
using namespace nanogui;
using namespace MISC;

SmokeSimulator::SmokeSimulator(string shader_dir, nanogui::Screen *screen, GLFWwindow *window)
{
    shader = make_shared<nanogui::GLShader>();
    shader->initFromFiles("Default",
                          shader_dir + "/point.vert",
                          shader_dir + "/point.frag");

    Vector3f init_velocity = Vector3f(0, 0.5, 0);
    Vector4f init_color = Vector4f(66 / 256.0f, 135 / 256.0f, 245 / 256.0f, 1.0f);
    emittor = Emittor(Vector3f(0, -1, 0), init_velocity, init_color);
    smoke = Smoke(400, 400, 400, 800, 800, 800);
    this->screen = screen;
    this->window = window;

    // Initialize camera

    Collada::CameraInfo camera_info;
    camera_info.hFov = 50;
    camera_info.vFov = 35;
    camera_info.nClip = 0.01;
    camera_info.fClip = 10000;

    // Try to intelligently figure out the camera target

    nanogui::Vector3f avg_p_position(0, 0, 0);

    for (auto &p : smoke.particles)
    {
        avg_p_position += p.pos / smoke.particles.size();
    }

    nanogui::Vector3f target(avg_p_position[0], avg_p_position[1] / 2,
                             avg_p_position[2]);
    nanogui::Vector3f c_dir(0., 0., 0.);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    canonical_view_distance = max(width, height) * 0.01f;
    scroll_rate = canonical_view_distance / 10;

    view_distance = canonical_view_distance * 2;
    min_view_distance = canonical_view_distance / 10.0;
    max_view_distance = canonical_view_distance * 20.0;

    // canonicalCamera is a copy used for view resets

    camera.place(target, acos(c_dir[1]), atan2(c_dir[0], c_dir[2]), view_distance,
                 min_view_distance, max_view_distance);
    canonicalCamera.place(target, acos(c_dir[1]), atan2(c_dir[0], c_dir[2]),
                          view_distance, min_view_distance, max_view_distance);

    screen_w = default_window_size(0);
    screen_h = default_window_size(1);

    camera.configure(camera_info, screen_w, screen_h);
    canonicalCamera.configure(camera_info, screen_w, screen_h);
}

void SmokeSimulator::resetCamera() { camera.copy_placement(canonicalCamera); }

nanogui::Matrix4f SmokeSimulator::getProjectionMatrix()
{
    nanogui::Matrix4f perspective;
    perspective.setZero();

    double cam_near = camera.near_clip();
    double cam_far = camera.far_clip();

    double theta = camera.v_fov() * PI / 360;
    double range = cam_far - cam_near;
    double invtan = 1. / tanf(theta);

    perspective(0, 0) = invtan / camera.aspect_ratio();
    perspective(1, 1) = invtan;
    perspective(2, 2) = -(cam_near + cam_far) / range;
    perspective(3, 2) = -1;
    perspective(2, 3) = -2 * cam_near * cam_far / range;
    perspective(3, 3) = 0;

    return perspective;
}
nanogui::Matrix4f SmokeSimulator::getViewMatrix()
{
    nanogui::Matrix4f lookAt;
    Matrix3f R;

    lookAt.setZero();

    // Convert CGL vectors to Eigen vectors
    // TODO: Find a better way to do this!

    nanogui::Vector3f c_pos = camera.position();
    nanogui::Vector3f c_udir = camera.up_dir();
    nanogui::Vector3f c_target = camera.view_point();

    Vector3f eye(c_pos[0], c_pos[1], c_pos[2]);
    Vector3f up(c_udir[0], c_udir[1], c_udir[2]);
    Vector3f target(c_target[0], c_target[1], c_target[2]);

    R.col(2) = (eye - target).normalized();
    R.col(0) = up.cross(R.col(2)).normalized();
    R.col(1) = R.col(2).cross(R.col(0));

    lookAt.topLeftCorner<3, 3>() = R.transpose();
    lookAt.topRightCorner<3, 1>() = -R.transpose() * eye;
    lookAt(3, 3) = 1.0f;

    return lookAt;
}
void SmokeSimulator::draw()
{
    shader->bind();

    // Prepare the camera projection matrix
    nanogui::Matrix4f model;
    model.setIdentity();
    nanogui::Matrix4f view = getViewMatrix();
    nanogui::Matrix4f projection = getProjectionMatrix();
    nanogui::Matrix4f viewProjection = projection * view;
    shader->setUniform("u_model", model);
    shader->setUniform("u_view_projection", viewProjection);

    // update particles positions
    smoke.generateParticles(emittor, 500);
    smoke.update(1 / frames_per_sec);
    nanogui::MatrixXf positions(4, smoke.particles.size());
    nanogui::MatrixXf colors(4, smoke.particles.size());
    nanogui::MatrixXf sizes(1, smoke.particles.size());
    nanogui::MatrixXf uv(2, smoke.particles.size());
    for (size_t idx_p = 0; const auto &p : smoke.particles)
    {

        positions.col(idx_p) << p.pos, 1;
        colors.col(idx_p) << p.color;
        sizes.col(idx_p) << p.sp->size;
        uv.col(idx_p) << p.uv;
        ++idx_p;
    }
    shader->uploadAttrib("in_position", positions, false);
    shader->uploadAttrib("in_color", colors, false);
    shader->uploadAttrib("in_size", sizes, false);
    shader->uploadAttrib("in_uv", uv, false);
    shader->drawArray(GL_POINTS, 0, smoke.particles.size());
}

void SmokeSimulator::initGUI()
{
    screen->initialize(window, true);
    std::cout << "Created screen" << std::endl;
    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Smoke Simulator Attributes");
    nanoguiWindow->setLayout(new BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Fill, 0, 20));

    gui->addGroup("Smoke Parameters");
    // ----------------------------------------------------------
    // Color Picker
    // ----------------------------------------------------------
    nanogui::ColorWheel *colorWheel = new nanogui::ColorWheel(nanoguiWindow);
    colorWheel->setFixedWidth(200);
    colorWheel->setColor(smoke.smoke_param->init_color);
    colorWheel->setCallback([this](const Color &c)
                            { this->smoke.smoke_param->init_color = c; });

    // ----------------------------------------------------------
    // Choose layer number
    // ----------------------------------------------------------
    nanogui::Widget *layerWidget = new nanogui::Widget(nanoguiWindow);
    layerWidget->setLayout(new GridLayout(nanogui::Orientation::Horizontal, 3, nanogui::Alignment::Minimum, 0, 5));
    Label *layerLabel = new Label(layerWidget, "Use Two layers neighbour cells");
    nanogui::CheckBox *layerCheckBox = new nanogui::CheckBox(layerWidget, "");
    layerLabel->setFixedWidth(150);
    layerCheckBox->setCallback([this](bool checked)
                               { this->smoke.smoke_param->two_layers = checked; });
    // ----------------------------------------------------------
    // Sliders
    // ----------------------------------------------------------
    nanogui::Widget *sliderGroups = new Widget(nanoguiWindow);
    sliderGroups->setLayout(new BoxLayout(nanogui::Orientation::Vertical, nanogui::Alignment::Minimum, 0, 10));
    gui->addWidget("", sliderGroups);
    // lifespan
    setupSlider(gui, sliderGroups, "Lifespan", smoke.smoke_param->lifespan, [this](double value)
                {this->smoke.smoke_param->lifespan.value = value; return true; });
    // size slider
    setupSlider(gui, sliderGroups, "Size", smoke.smoke_param->size, [this](double value)
                {this->smoke.smoke_param->size.value = value; return true; });
    // smoothed particle length multiplier
    setupSlider(gui, sliderGroups, "Smoothed Len", smoke.smoke_param->L, [this](double value)
                {this->smoke.smoke_param->L.value = value; return true; });
    // smoothed particle length multiplier
    setupSlider(gui, sliderGroups, "Mass", smoke.smoke_param->M, [this](double value)
                {this->smoke.smoke_param->M.value = value; return true; });
    // vortical velocity confinement
    setupSlider(gui, sliderGroups, "Vort Epsilon", smoke.smoke_param->vorticity_epsilon, [this](double value)
                {this->smoke.smoke_param->vorticity_epsilon.value = value; return true; });
    // damping multiplier
    setupSlider(gui, sliderGroups, "Damping", smoke.smoke_param->damping, [this](double value)
                {this->smoke.smoke_param->damping.value = value; return true; });
    // ambient temperature
    setupSlider(gui, sliderGroups, "Ambient Temp", smoke.smoke_param->ambient_temp, [this](double value)
                {this->smoke.smoke_param->ambient_temp.value = value; return true; });
    // fluid stiffness
    setupSlider(gui, sliderGroups, "Fluid Stiffness", smoke.smoke_param->fluid_stiffness, [this](double value)
                {this->smoke.smoke_param->fluid_stiffness.value = value; return true; });
    // buoyancy coefficient
    setupSlider(gui, sliderGroups, "Buoyancy Coef", smoke.smoke_param->buoyancy_coefficient, [this](double value)
                {this->smoke.smoke_param->buoyancy_coefficient.value = value; return true; });
    // thermal diffusivity
    setupSlider(gui, sliderGroups, "Thermal Expansion Coef", smoke.smoke_param->thermal_expansion_coefficient, [this](double value)
                {this->smoke.smoke_param->thermal_expansion_coefficient.value = value; return true; });
    // air drag coefficient
    setupSlider(gui, sliderGroups, "Air Drag Coef", smoke.smoke_param->air_drag_coefficient, [this](double value)
                {this->smoke.smoke_param->air_drag_coefficient.value = value; return true; });
    // viscosity coefficient
    setupSlider(gui, sliderGroups, "Viscosity Coef", smoke.smoke_param->dynamic_viscosity_coefficient, [this](double value)
                {this->smoke.smoke_param->dynamic_viscosity_coefficient.value = value; return true; });

    screen->setVisible(true);
    screen->performLayout();
    std::cout << "GUI initialized" << std::endl;
};

// ----------------------------------------------------------------------------
// GUI Setup Helpers
// ----------------------------------------------------------------------------
void SmokeSimulator::setupSlider(
    nanogui::FormHelper *gui,
    nanogui::Widget *sliderGroups,
    std::string label,
    Parameter &param,
    std::function<bool(double)> callback)
{

    nanogui::Widget *rowWidget = new nanogui::Widget(sliderGroups);
    rowWidget->setLayout(new GridLayout(nanogui::Orientation::Horizontal, 3, nanogui::Alignment::Maximum, 0, 5));
    int labelLen = 100;
    Label *sliderLabel = new Label(rowWidget, label);
    sliderLabel->setFixedWidth(labelLen);
    Slider *slider = new Slider(rowWidget);
    slider->setValue(param);
    slider->setFixedWidth(150);
    slider->setRange(std::make_pair(param.lower, param.upper));
    // slider->setCallback([this](float value) {});
    Label *sliderValueLabel = new Label(rowWidget, std::to_string(param));
    slider->setCallback([this, sliderValueLabel, callback](double value)
                        { 
                        sliderValueLabel->setCaption(std::to_string(value)); 
                        callback(value); });
}
// ----------------------------------------------------------------------------
// EVENT HANDLING
// ----------------------------------------------------------------------------

bool SmokeSimulator::cursorPosCallbackEvent(double x, double y)
{
    if (left_down && !middle_down && !right_down)
    {
        if (ctrl_down)
        {
            mouseRightDragged(x, y);
        }
        else
        {
            mouseLeftDragged(x, y);
        }
    }
    else if (!left_down && !middle_down && right_down)
    {
        mouseRightDragged(x, y);
    }
    else if (!left_down && !middle_down && !right_down)
    {
        mouseMoved(x, y);
    }

    mouse_x = x;
    mouse_y = y;

    return true;
}

bool SmokeSimulator::mouseButtonCallbackEvent(int button, int action,
                                              int modifiers)
{
    switch (action)
    {
    case GLFW_PRESS:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            left_down = true;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            middle_down = true;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            right_down = true;
            break;
        }
        return true;

    case GLFW_RELEASE:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            left_down = false;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            middle_down = false;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            right_down = false;
            break;
        }
        return true;
    }

    return false;
}

void SmokeSimulator::mouseMoved(double x, double y) { y = screen_h - y; }

void SmokeSimulator::mouseLeftDragged(double x, double y)
{
    float dx = x - mouse_x;
    float dy = y - mouse_y;

    camera.rotate_by(-dy * (PI / screen_h), -dx * (PI / screen_w));
}

void SmokeSimulator::mouseRightDragged(double x, double y)
{
    camera.move_by(mouse_x - x, y - mouse_y, canonical_view_distance);
}

bool SmokeSimulator::keyCallbackEvent(int key, int scancode, int action,
                                      int mods)
{
    ctrl_down = (bool)(mods & GLFW_MOD_CONTROL);

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            is_alive = false;
            break;
        case 'r':
        // case 'R':
        //   cloth->reset();
        //   break;
        case ' ':
            resetCamera();
            break;
        case 'p':
        case 'P':
            is_paused = !is_paused;
            break;
        case 'n':
        case 'N':
            if (is_paused)
            {
                is_paused = false;
                draw();
                is_paused = true;
            }
            break;
        }
    }

    return true;
}

bool SmokeSimulator::dropCallbackEvent(int count, const char **filenames)
{
    return true;
}

bool SmokeSimulator::scrollCallbackEvent(double x, double y)
{
    camera.move_forward(y * scroll_rate);
    return true;
}

bool SmokeSimulator::resizeCallbackEvent(int width, int height)
{
    screen_w = width;
    screen_h = height;

    camera.set_screen_size(screen_w, screen_h);
    return true;
}