#ifndef DRAW_LOOP
#define DRAW_LOOP

#include "scene.hpp"
#include "gui.hpp"

class DrawLoop
{
protected:
    bool stop = false;
    GLFWwindow *window = nullptr;
    int *windowWidth = nullptr, *windowHeight = nullptr;

    virtual void init();
    virtual void resize();
    virtual void inputs();
    virtual void update();
    virtual void draw();
    virtual void terminate();

public:
    Scene *scene = nullptr;
    std::map<std::string, gui::Master *> sceneGUI;

    DrawLoop(GLFWwindow *window, int *windowWidth, int *windowHeight);
    ~DrawLoop();

    void operator()();
};

#endif