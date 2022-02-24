#include "../../../header/gl/gui_utility.hpp"

int gui_utility::scrollY = 0;
void gui_utility::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (yoffset > 0)
        scrollY = 1;
    else if (yoffset < 0)
        scrollY = -1;
    else
        scrollY = 0;
}

unsigned int gui_utility::charCodePoint = 0;
void gui_utility::character_callback(GLFWwindow *window, unsigned int codePoint)
{
    charCodePoint = codePoint;
}

void gui_utility::reset()
{
    scrollY = charCodePoint = 0;
}