#include "../../../header/gl/gui.hpp"

gui::native::Button::Button(glm::vec4 color, int borderRadius, std::function<void()> onClick, Constraints *constraints) : Container(color, borderRadius, constraints)
{
    defaultColor = clickColor = color;
    clickCallback = onClick;
}
gui::native::Button::Button(glm::vec4 color, int borderWidth, glm::vec4 borderColor, std::function<void()> onClick, Constraints *constraints) : Container(color, borderWidth, borderColor, constraints)
{
    defaultColor = clickColor = color;
    clickCallback = onClick;
}

void gui::native::Button::changeColor(int color)
{
    for (Vertex2D &v : poly[0]->vertices)
        v.color = color == 0 ? defaultColor : clickColor;
    poly[0]->applyVertexChange();
}

void gui::native::Button::setClickColor(glm::vec4 color)
{
    clickColor = color;
}
void gui::native::Button::setDefaultColor(glm::vec4 color)
{
    defaultColor = color;
    changeColor(0);
}

bool gui::native::Button::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
    if (!clickCallback)
        return false;
    bool mouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (mouseDown)
    {
        mouseDrag = true;
        changeColor(1);
    }
    else if (mouseDrag)
    {
        mouseDrag = false;
        float maxWidth = layout->w * windowWidth, maxHeight = layout->h * windowHeight;
        if ((relativeMousePos_x >= 0 && relativeMousePos_x <= maxWidth) && (relativeMousePos_y >= 0 && relativeMousePos_y <= maxHeight))
        {
            clickCallback();
            if (!keepClickColorAfterClick)
                changeColor(0);
        }
        else
            changeColor(0);
    }
    return mouseDown;
}