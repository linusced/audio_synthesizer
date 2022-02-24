#include "../../../header/gl/gui.hpp"

gui::Font::Font(std::string filePath)
{
    FILE *fontFile = fopen(filePath.c_str(), "rb");
    fseek(fontFile, 0, SEEK_END);
    unsigned long size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    this->buffer = (unsigned char *)malloc(size);
    fread(this->buffer, size, 1, fontFile);
    fclose(fontFile);
}

gui::Element::Element(Constraints *constraints)
{
    c = constraints;
}
gui::Element::~Element()
{
    for (Polygon *p : poly)
        delete p;

    if (layout)
        delete layout;
    for (auto ch : children)
        delete ch;
}
void gui::Element::addChild(Element *child)
{
    children.push_back(child);
}
void gui::Element::resize(Layout *parentLayout, int &windowWidth, int &windowHeight)
{
    layout = c->getLayout(parentLayout, windowWidth, windowHeight);
    resizePoly(windowWidth, windowHeight);

    for (Element *ch : children)
        ch->resize(layout, windowWidth, windowHeight);
}
void gui::Element::draw()
{
    for (Polygon *p : poly)
        p->draw();
    for (Element *ch : children)
        if (ch->draw_b)
            ch->draw();
}
gui::Layout gui::Element::getRelativePixelCoords(Layout *parentLayout, int &windowWidth, int &windowHeight)
{
    float x = layout->x * windowWidth, y = layout->y * windowHeight, w = layout->w * windowWidth, h = layout->h * windowHeight,
          parentX = parentLayout->x * windowWidth, parentY = parentLayout->y * windowHeight;
    return {x - parentX, y - parentY, w, h};
}
bool gui::Element::mouseDragActive()
{
    bool active = mouseDrag;
    if (!active)
        for (Element *ch : children)
            if (ch->mouseDragActive())
                active = true;
    return active;
}

void gui::Element::resizePoly(int &windowWidth, int &windowHeight)
{
    return;
}
bool gui::Element::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
    for (Element *ch : children)
        if (ch->mouseDragActive())
        {
            Layout pixelCoords = ch->getRelativePixelCoords(layout, windowWidth, windowHeight);
            ch->inputs(window, relativeMousePos_x - pixelCoords.x, relativeMousePos_y - pixelCoords.y, windowWidth, windowHeight);
            return true;
        }
    for (auto i = children.rbegin(); i != children.rend(); i++)
    {
        Element *ch = i[0];
        if (ch->draw_b && ch->inputs_b)
        {
            Layout pixelCoords = ch->getRelativePixelCoords(layout, windowWidth, windowHeight);
            if ((relativeMousePos_x >= pixelCoords.x && relativeMousePos_x <= pixelCoords.x + pixelCoords.w) && (relativeMousePos_y >= pixelCoords.y && relativeMousePos_y <= pixelCoords.y + pixelCoords.h))
            {
                if (ch->inputs(window, relativeMousePos_x - pixelCoords.x, relativeMousePos_y - pixelCoords.y, windowWidth, windowHeight))
                    return true;
            }
        }
    }
    return false;
}

gui::Master::~Master()
{
    for (auto con : constraints_named)
        delete con.second;
    for (auto ch : children)
        delete ch;
}

void gui::Master::addChild(Element *child)
{
    children.push_back(child);
}
void gui::Master::resize(int &windowWidth, int &windowHeight)
{
    Layout *parentLayout = Constraints::getWindowLayout();
    for (Element *ch : children)
        ch->resize(parentLayout, windowWidth, windowHeight);
}
void gui::Master::draw()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    for (Element *ch : children)
        if (ch->draw_b)
            ch->draw();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}
bool gui::Master::inputs(GLFWwindow *window, int &windowWidth, int &windowHeight)
{
    Layout *parentLayout = Constraints::getWindowLayout();
    double mousePos_x, mousePos_y;
    glfwGetCursorPos(window, &mousePos_x, &mousePos_y);
    if (prevBlock)
        for (Element *ch : children)
            if (ch->mouseDragActive())
            {
                Layout pixelCoords = ch->getRelativePixelCoords(parentLayout, windowWidth, windowHeight);
                ch->inputs(window, mousePos_x - pixelCoords.x, mousePos_y - pixelCoords.y, windowWidth, windowHeight);
                prevBlock = true;
                return true;
            }
    for (auto i = children.rbegin(); i != children.rend(); i++)
    {
        Element *ch = i[0];
        if (ch->draw_b && ch->inputs_b)
        {
            Layout pixelCoords = ch->getRelativePixelCoords(parentLayout, windowWidth, windowHeight);
            if ((mousePos_x >= pixelCoords.x && mousePos_x <= pixelCoords.x + pixelCoords.w) && (mousePos_y >= pixelCoords.y && mousePos_y <= pixelCoords.y + pixelCoords.h))
            {
                if (ch->inputs(window, mousePos_x - pixelCoords.x, mousePos_y - pixelCoords.y, windowWidth, windowHeight))
                {
                    prevBlock = true;
                    return true;
                }
            }
        }
    }
    if (prevBlock && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
        prevBlock = false;
    return prevBlock;
}

void gui::Master::saveNamedChild(std::string name, Element *child, bool add)
{
    children_named[name] = child;
    if (add)
        children.push_back(child);
}
gui::Element *gui::Master::getNamedChild(std::string name)
{
    auto i = children_named.find(name);
    if (i != children_named.end())
        return i->second;
    return nullptr;
}
void gui::Master::saveConstraints(std::string name, Constraints *con)
{
    constraints_named[name] = con;
}
gui::Constraints *gui::Master::getConstraints(std::string name)
{
    auto i = constraints_named.find(name);
    if (i != constraints_named.end())
        return i->second;
    return nullptr;
}