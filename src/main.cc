#include <iostream>
#include <vector>
#include <memory>
#include <string>

#include "smoke_simulator.hh"
#include "collision/plane.hh"

SmokeSimulator *sim = nullptr;
nanogui::Screen *screen = nullptr;
GLFWwindow *window = nullptr;

void error_callback(int error, const char *description)
{
    puts(description);
}

void setGLFWCallbacks()
{
    glfwSetCursorPosCallback(window, [](GLFWwindow *, double x, double y)
                             {
    if (!screen->cursorPosCallbackEvent(x, y)) {
      sim->cursorPosCallbackEvent(x / screen->pixelRatio(),
                                  y / screen->pixelRatio());
    } });

    glfwSetMouseButtonCallback(
        window, [](GLFWwindow *, int button, int action, int modifiers)
        {
        if (!screen->mouseButtonCallbackEvent(button, action, modifiers) ||
            action == GLFW_RELEASE) {
          sim->mouseButtonCallbackEvent(button, action, modifiers);
        } });

    glfwSetKeyCallback(
        window, [](GLFWwindow *, int key, int scancode, int action, int mods)
        {
        if (!screen->keyCallbackEvent(key, scancode, action, mods)) {
          sim->keyCallbackEvent(key, scancode, action, mods);
        } });

    glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint)
                        { screen->charCallbackEvent(codepoint); });

    glfwSetDropCallback(window,
                        [](GLFWwindow *, int count, const char **filenames)
                        {
                            screen->dropCallbackEvent(count, filenames);
                            sim->dropCallbackEvent(count, filenames);
                        });

    glfwSetScrollCallback(window, [](GLFWwindow *, double x, double y)
                          {
    if (!screen->scrollCallbackEvent(x, y)) {
      sim->scrollCallbackEvent(x, y);
    } });

    glfwSetFramebufferSizeCallback(window,
                                   [](GLFWwindow *, int width, int height)
                                   {
                                       screen->resizeCallbackEvent(width, height);
                                       sim->resizeCallbackEvent(width, height);
                                   });
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
    window = glfwCreateWindow(1000, 1000, "Smoke Simulator", nullptr, nullptr);

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
    screen = new nanogui::Screen();
    std::cout << "Completed Initialization" << std::endl;

    sim = new SmokeSimulator(shader_path, screen, window);
    sim->initGUI();
    setGLFWCallbacks();

    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    CollisionObject* collisionPlaneOne = new Plane(Vector3f(0, 12, 0), Vector3f(0, 1, 0), 0.5);
    sim->loadCollisionObject(collisionPlaneOne);

    double prevTime = 0.0;
    double curTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        curTime = glfwGetTime();
        timeDiff = curTime - prevTime;
        counter++;
        if (timeDiff >= 1.0 / 30.0) {
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle = "Smoke Sim - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = curTime;
            counter = 0;

        }

        sim->draw();

        screen->drawContents();
        screen->drawWidgets();

        glfwSwapBuffers(window);
    }

    delete screen;
    delete collisionPlaneOne;
    return 0;
}