#ifndef GUI_UTILITY
#define GUI_UTILITY

#include <GLFW/glfw3.h>

namespace gui_utility
{
    extern int scrollY;
    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

    extern unsigned int charCodePoint;
    void character_callback(GLFWwindow *window, unsigned int codePoint);

    void reset();
};

#endif