#include "../../header/gl/draw_loop.hpp"

DrawLoop::DrawLoop(GLFWwindow *window, int *windowWidth, int *windowHeight)
{
    this->window = window;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}
DrawLoop::~DrawLoop()
{
    for (auto g : sceneGUI)
        delete g.second;
}

void DrawLoop::operator()()
{
    init();

    glfwSetScrollCallback(window, gui_utility::scroll_callback);
    glfwSetCharCallback(window, gui_utility::character_callback);

    int prevWidth = 0, prevHeight = 0;
    while (!glfwWindowShouldClose(window) && !stop)
    {
        glfwGetWindowSize(window, windowWidth, windowHeight);
        if (prevWidth != windowWidth[0] || prevHeight != windowHeight[0])
        {
            resize();
            prevWidth = windowWidth[0];
            prevHeight = windowHeight[0];
        }

        update();
        inputs();
        draw();

        gui_utility::reset();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    terminate();
}

void DrawLoop::init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void DrawLoop::resize()
{
    if (scene)
        scene->resize();
    for (auto g : sceneGUI)
        g.second->resize(windowWidth[0], windowHeight[0]);
}
void DrawLoop::inputs()
{
    bool guiInput = false;
    for (auto g : sceneGUI)
        if (g.second->inputs(window, windowWidth[0], windowHeight[0]))
            guiInput = true;

    if (!guiInput)
        if (scene)
            scene->inputs();
}
void DrawLoop::update() {}
void DrawLoop::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (scene)
        scene->draw();
    for (auto g : sceneGUI)
        g.second->draw();
}
void DrawLoop::terminate()
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}