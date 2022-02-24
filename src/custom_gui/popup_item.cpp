#include "../../header/custom_gui/popup_item.hpp"

customGUI::PopupItem::PopupItem(std::string text, int index, std::function<void(int)> *callback, glm::vec4 bgColor, glm::vec4 clickColor, glm::vec3 textColor, gui::Font *font, gui::Constraints *btnText, gui::Constraints *constraints, bool centerText)
    : gui::Element(constraints)
{
    this->index = index;
    this->callback = callback;
    this->bgColor = bgColor;
    this->clickColor = clickColor;

    std::vector<Vertex2D> vertices;
    for (int i = 0; i < 4; i++)
        vertices.push_back({{0, 0}, bgColor});

    poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));

    textElement = new gui::native::Text(font, text, 35, textColor, false, centerText ? gui::ALIGNMENT_CENTER_H : gui::ALIGNMENT_LEFT_H, gui::ALIGNMENT_CENTER_V, btnText);
    addChild(textElement);
}

void customGUI::PopupItem::PopupItem::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}

void customGUI::PopupItem::changeColor(int color)
{
    for (Vertex2D &v : poly[0]->vertices)
        v.color = color == 0 ? bgColor : clickColor;
    poly[0]->applyVertexChange();
}

bool customGUI::PopupItem::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
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
            callback->operator()(index);

        changeColor(0);
    }
    return mouseDown;
}