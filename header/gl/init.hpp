#ifndef INIT
#define INIT

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool init(GLFWwindow *&window, int windowWidth, int windowHeight, const char *windowName)
{
    if (!glfwInit())
    {
        std::cout << "\033[1;31mError: Failed to initialize GLFW!\033[0m\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
    if (!window)
    {
        std::cout << "\033[1;31mError: Failed to open window!\033[0m\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cout << "\033[1;31mError: Failed to load GL (via glad)!\033[0m\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    return true;
}

#endif