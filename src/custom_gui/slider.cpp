#include "../../header/custom_gui/slider.hpp"

customGUI::Slider::Slider(double min, double max, double value, std::function<void(double &)> valueChangeCallback, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->min = min;
    this->max = max;
    this->value = value;
    this->callback = valueChangeCallback;

    std::vector<Vertex2D> vertices;
    for (int i = 0; i < 4; i++)
        vertices.push_back({{0, 0}, glm::vec4(1)});
    poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));

    for (int i = 0; i < 4; i++)
        vertices[i].color = {0, .5, 1, 1};
    poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));

    std::vector<unsigned int> circleIndices;
    for (int i = 0; i < 22; i++)
        vertices.push_back({{0, 0}, {0, .5, 1, 1}});

    for (int i = 1; i < vertices.size() - 1; i++)
    {
        circleIndices.push_back(i);
        circleIndices.push_back(0);
        circleIndices.push_back(i + 1);
    }
    circleIndices.push_back(vertices.size() - 1);
    circleIndices.push_back(0);
    circleIndices.push_back(1);

    poly.push_back(new Polygon(vertices, circleIndices));
}

void customGUI::Slider::resizePoly(int &windowWidth, int &windowHeight)
{
    if (!this->windowWidth)
        this->windowWidth = &windowWidth;
    if (!this->windowHeight)
        this->windowHeight = &windowHeight;

    double percentage = (value / (max - min)) - (min / (max - min));

    poly[0]->vertices[0].position = {layout->x, layout->y + layout->h * .25};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y + layout->h * .25};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h * .75};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h * .75};

    poly[0]->applyVertexChange();

    poly[1]->vertices[0].position = {layout->x, layout->y + layout->h * .25};
    poly[1]->vertices[1].position = {layout->x + layout->w * percentage, layout->y + layout->h * .25};
    poly[1]->vertices[2].position = {layout->x + layout->w * percentage, layout->y + layout->h * .75};
    poly[1]->vertices[3].position = {layout->x, layout->y + layout->h * .75};

    for (int i = 0; i < 4; i++)
        poly[1]->vertices[i].color.g = mouseDrag ? .4 : .5;

    poly[1]->applyVertexChange();

    float r_x = (layout->h * windowHeight / 2) / windowWidth, r_y = layout->h / 2;
    glm::vec2 circleCenter = {layout->x + layout->w * percentage, layout->y + layout->h / 2};
    poly[2]->vertices[0].position = circleCenter;

    int size = poly[2]->vertices.size();
    for (int i = 1; i < size; i++)
        poly[2]->vertices[i].position = circleCenter + glm::vec2(r_x * cos(((i - 1.0) / (size - 1.0)) * (M_PI * 2)), r_y * sin(((i - 1.0) / (size - 1.0)) * (M_PI * 2)));

    for (int i = 0; i < size; i++)
        poly[2]->vertices[i].color.g = mouseDrag ? .4 : .5;

    poly[2]->applyVertexChange();
}

bool customGUI::Slider::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
    if (mouseDrag)
    {
        double newValue = (relativeMousePos_x / (layout->w * windowWidth)) * (max - min) + min;
        if (newValue < min)
            newValue = min;
        else if (newValue > max)
            newValue = max;

        if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        {
            mouseDrag = false;
            if (callback)
                callback(newValue);
        }
        else if (callback && callbackOnMouseDown)
            callback(newValue);

        setValue(newValue);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        mouseDrag = true;

    return mouseDrag;
}

double customGUI::Slider::getValue()
{
    return value;
}
void customGUI::Slider::setValue(double newValue)
{
    if (newValue < min)
        value = min;
    else if (newValue > max)
        value = max;
    else
        value = newValue;

    resizePoly(*windowWidth, *windowHeight);
}
void customGUI::Slider::setMin(double newMin)
{
    min = newMin;
    resizePoly(*windowWidth, *windowHeight);
}
void customGUI::Slider::setMax(double newMax)
{
    max = newMax;
    resizePoly(*windowWidth, *windowHeight);
}