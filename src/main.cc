#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "smoke_simulator.hh"

void error_callback(int error, const char *description)
{
    puts(description);
}

int main(int argc, char **argv)
{
    std::string shader_path = argv[1];

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        return 0;
    }
    std::cout << "Init Complete" << std::endl;
    glfwSetTime(0);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object
    auto *window = glfwCreateWindow(1000, 1000, "Smoke Simulator", nullptr, nullptr);

    std::cout << "Created window" << std::endl;

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Could not initialize GLAD!");
    }

    glGetError(); // pull and ignore unhandled errors

    glClearColor(0.2f, 0.25f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
    glfwSwapBuffers(window);

    // Create a nanogui screen
    auto *screen = new nanogui::Screen();
    std::cout << "Completed Initialization" << std::endl;

    SmokeSimulator sim(shader_path, screen, window);
    sim.initGUI();

        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sim.draw();

        screen->drawContents();
        screen->drawWidgets();

        glfwSwapBuffers(window);
    }
    delete screen;
    return 0;
}