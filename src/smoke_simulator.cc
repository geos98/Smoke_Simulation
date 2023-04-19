#include <nanogui/nanogui.h>
#include "smoke_simulator.hh"

using namespace nanogui;

SmokeSimulator::SmokeSimulator(std::string shader_dir, nanogui::Screen *screen, GLFWwindow *window)
{
    shader = std::make_shared<nanogui::GLShader>();
    shader->initFromFiles("Default",
                          shader_dir + "/point.vert",
                          shader_dir + "/point.frag");

    smoke = new Smoke();
    this->screen = screen;
    this->window = window;
}

void SmokeSimulator::draw()
{
    smoke->update(1 / 100.0);

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
void SmokeSimulator::initGUI()
{
    FormHelper *gui = new FormHelper(screen);
    nanogui::ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(1, 1), "Form helper example");
    gui->addGroup("Basic types");
    // gui->addVariable("bool", true)->setTooltip("Test tooltip.");

    gui->addVariable("string", strval);

    screen->initialize(window, true);
    std::cout << "Created screen" << std::endl;

    screen->setVisible(true);
    screen->performLayout();
}

// void SmokeSimulator::initGUI()
// {
//     Window *window;

//     window = new Window(screen, "Simulation");
//     window->setPosition(Vector2i(1, 1));
//     window->setLayout(new GroupLayout(1, 1, 1, 1));

//     // new Label(window, "Spring types", "sans-bold");

//     // {
//     //     Button *b = new Button(window, "structural");
//     //     b->setFlags(Button::ToggleButton);
//     //     b->setPushed(cp->enable_structural_constraints);
//     //     b->setFontSize(14);
//     //     b->setChangeCallback(
//     //         [this](bool state)
//     //         { cp->enable_structural_constraints = state; });

//     //     b = new Button(window, "shearing");
//     //     b->setFlags(Button::ToggleButton);
//     //     b->setPushed(cp->enable_shearing_constraints);
//     //     b->setFontSize(14);
//     //     b->setChangeCallback(
//     //         [this](bool state)
//     //         { cp->enable_shearing_constraints = state; });

//     //     b = new Button(window, "bending");
//     //     b->setFlags(Button::ToggleButton);
//     //     b->setPushed(cp->enable_bending_constraints);
//     //     b->setFontSize(14);
//     //     b->setChangeCallback(
//     //         [this](bool state)
//     //         { cp->enable_bending_constraints = state; });
//     // }

//     // Mass-spring parameters

//     new Label(window, "Parameters", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
//         layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//         layout->setSpacing(0, 10);
//         panel->setLayout(layout);

//         new Label(panel, "density :", "sans-bold");

//         FloatBox<double> *fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(10);
//         fb->setUnits("g/cm^2");
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "ks :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(1);
//         fb->setUnits("N/m");
//         fb->setSpinnable(true);
//         fb->setMinValue(0);
//         fb->setCallback([this](float value) { /* cp->ks = value; */ });
//     }

//     // Simulation constants

//     new Label(window, "Simulation", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
//         layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//         layout->setSpacing(0, 10);
//         panel->setLayout(layout);

//         new Label(panel, "frames/s :", "sans-bold");

//         IntBox<int> *fsec = new IntBox<int>(panel);
//         fsec->setEditable(true);
//         fsec->setFixedSize(Vector2i(100, 20));
//         fsec->setFontSize(14);
//         fsec->setValue(frames_per_sec);
//         fsec->setSpinnable(true);
//         fsec->setCallback([this](int value)
//                           { frames_per_sec = value; });

//         new Label(panel, "steps/frame :", "sans-bold");

//         IntBox<int> *num_steps = new IntBox<int>(panel);
//         num_steps->setEditable(true);
//         num_steps->setFixedSize(Vector2i(100, 20));
//         num_steps->setFontSize(14);
//         num_steps->setValue(2);
//         num_steps->setSpinnable(true);
//         num_steps->setMinValue(0);
//         num_steps->setCallback([this](int value) { /* simulation_steps = value; */ });
//     }

//     // Damping slider and textbox

//     new Label(window, "Damping", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         panel->setLayout(
//             new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));

//         Slider *slider = new Slider(panel);
//         slider->setValue(7);
//         slider->setFixedWidth(105);

//         TextBox *percentage = new TextBox(panel);
//         percentage->setFixedWidth(75);
//         percentage->setValue(to_string(12));
//         percentage->setUnits("%");
//         percentage->setFontSize(14);

//         slider->setCallback([percentage](float value)
//                             { percentage->setValue(std::to_string(value)); });
//         slider->setFinalCallback([&](float value)
//                                  {
//                                      // cout << "Final slider value: " << (int)(value * 100) << endl;
//                                  });
//     }

//     // Gravity

//     new Label(window, "Gravity", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
//         layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//         layout->setSpacing(0, 10);
//         panel->setLayout(layout);

//         new Label(panel, "x :", "sans-bold");

//         FloatBox<double> *fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(20);
//         fb->setUnits("m/s^2");
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "y :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(20);
//         fb->setUnits("m/s^2");
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "z :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(/* gravity.gravity.z */ 1);
//         fb->setUnits("m/s^2");
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) { /* gravity.gravity.z */ });
//     }

//     // Wind
//     new Label(window, "Wind Speed", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         panel->setLayout(
//             new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));

//         Slider *slider = new Slider(panel);
//         slider->setValue(20);
//         slider->setFixedWidth(105);

//         TextBox *percentage = new TextBox(panel);
//         percentage->setFixedWidth(75);
//         percentage->setValue(to_string(/* wind.wind_intensity */ 1));
//         percentage->setUnits("m/s");
//         percentage->setFontSize(14);

//         slider->setCallback([percentage](float value)
//                             { percentage->setValue(std::to_string(value)); });
//         slider->setFinalCallback([&](float value) { /* wind.wind_intensity = (double)value; */ });
//     }

//     new Label(window, "Wind Direction", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
//         layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//         layout->setSpacing(0, 10);
//         panel->setLayout(layout);

//         new Label(panel, "x :", "sans-bold");

//         FloatBox<double> *fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(1);
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "y :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(1);
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "z :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(1);
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) { /* wind.wind_direction.z = value; */ });
//     }

//     window = new Window(screen, "Appearance");
//     window->setPosition(Vector2i(15, 15));
//     window->setLayout(new GroupLayout(15, 6, 14, 5));

//     // Appearance

//     // {

//     //     ComboBox *cb = new ComboBox(window, shaders_combobox_names);
//     //     cb->setFontSize(14);
//     //     cb->setCallback(
//     //         [this, screen](int idx)
//     //         { active_shader_idx = idx; });
//     //     cb->setSelectedIndex(active_shader_idx);
//     // }

//     // Shader Parameters

//     // new Label(window, "Color", "sans-bold");

//     // {
//     //     ColorWheel *cw = new ColorWheel(window, color);
//     //     cw->setColor(this->color);
//     //     cw->setCallback(
//     //         [this](const nanogui::Color &color)
//     //         { this->color = color; });
//     // }

//     new Label(window, "Parameters", "sans-bold");

//     {
//         Widget *panel = new Widget(window);
//         GridLayout *layout =
//             new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 5, 5);
//         layout->setColAlignment({Alignment::Maximum, Alignment::Fill});
//         layout->setSpacing(0, 10);
//         panel->setLayout(layout);

//         new Label(panel, "Normal :", "sans-bold");

//         FloatBox<double> *fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(2);
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});

//         new Label(panel, "Height :", "sans-bold");

//         fb = new FloatBox<double>(panel);
//         fb->setEditable(true);
//         fb->setFixedSize(Vector2i(100, 20));
//         fb->setFontSize(14);
//         fb->setValue(1);
//         fb->setSpinnable(true);
//         fb->setCallback([this](float value) {});
//     }
// }
