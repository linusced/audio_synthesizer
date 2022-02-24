#include "../../../header/gl/gui.hpp"

gui::native::TextInput::TextInput(Font *font, std::string defaultText, int fontSize, glm::vec3 textColor, std::function<void(std::string &)> onInputEscape, std::function<void(std::string &)> onInputEnter, Constraints *constraints) : Text(font, defaultText, fontSize, textColor, false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, constraints)
{
    escapeCallback = onInputEscape;
    enterCallback = onInputEnter;

    std::vector<Vertex2D> vertices;
    for (int i = 0; i < 4; i++)
        vertices.push_back({{0, 0}, textColor});
    poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));
}

void gui::native::TextInput::textCursor(bool active, int &windowWidth, int &windowHeight)
{
    double currentTime = glfwGetTime();
    if (active && (currentTime - lastChange < .5 || fmod(currentTime, 1) < .5))
    {
        int textSize = getText().size();
        float textWidth{0}, totalTextWidth = (float)getTextWidth() / windowWidth;
        if (textCursorOffset > 0)
        {
            for (int i = 0; i < textSize - textCursorOffset; i++)
                textWidth += getCharWidth(i);
            textWidth *= totalTextWidth;
        }
        else
            textWidth = totalTextWidth;
        float w = 2.0 / windowWidth, x = layout->x + (layout->w / 2) - (totalTextWidth / 2) + textWidth;

        poly[1]->vertices[0].position = {x, layout->y + (w * 2)};
        poly[1]->vertices[1].position = {x + w, layout->y + (w * 2)};
        poly[1]->vertices[2].position = {x + w, layout->y + layout->h - (w * 2)};
        poly[1]->vertices[3].position = {x, layout->y + layout->h - (w * 2)};
    }
    else
        for (int i = 0; i < 4; i++)
            poly[1]->vertices[i].position = {0, 0};

    poly[1]->applyVertexChange();
}

bool gui::native::TextInput::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
    textCursor(mouseDrag, windowWidth, windowHeight);

    if (!glfwGetKey(window, GLFW_KEY_BACKSPACE))
        backspaceKeyDown = false;
    if (arrowLeftKeyDown && !glfwGetKey(window, GLFW_KEY_LEFT))
        arrowLeftKeyDown = false;
    if (arrowRightKeyDown && !glfwGetKey(window, GLFW_KEY_RIGHT))
        arrowRightKeyDown = false;

    double currentTime = glfwGetTime();
    if (!mouseDrag && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        mouseDrag = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        lastChange = currentTime;
    }
    else if (mouseDrag)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseDrag = false;
            if (escapeCallback)
            {
                std::string text = getText();
                escapeCallback(text);
                setText(text, getFontSize(), getColor(), windowWidth, windowHeight);
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_ENTER) || glfwGetKey(window, GLFW_KEY_KP_ENTER))
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseDrag = false;
            if (enterCallback)
            {
                std::string text = getText();
                enterCallback(text);
                setText(text, getFontSize(), getColor(), windowWidth, windowHeight);
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            if ((!arrowLeftKeyDown || currentTime - arrowLeftKeyDownTime > .5) && textCursorOffset < getText().size())
            {
                textCursorOffset++;
                if (!arrowLeftKeyDown)
                    arrowLeftKeyDownTime = currentTime;
            }
            arrowLeftKeyDown = true;
            lastChange = currentTime;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            if ((!arrowRightKeyDown || currentTime - arrowRightKeyDownTime > .5) && textCursorOffset > 0)
            {
                textCursorOffset--;
                if (!arrowRightKeyDown)
                    arrowRightKeyDownTime = currentTime;
            }
            arrowRightKeyDown = true;
            lastChange = currentTime;
        }
        else if (glfwGetKey(window, GLFW_KEY_BACKSPACE))
        {
            std::string text = getText();
            if ((!backspaceKeyDown || currentTime - backspaceKeyDownTime > .5) && text.size() - textCursorOffset > 0)
            {
                if (textCursorOffset == 0)
                    text = text.substr(0, text.size() - 1);
                else
                    text = text.substr(0, text.size() - 1 - textCursorOffset) + text.substr(text.size() - textCursorOffset);
                setText(text, getFontSize(), getColor(), windowWidth, windowHeight);
                if (!backspaceKeyDown)
                    backspaceKeyDownTime = currentTime;
            }
            backspaceKeyDown = true;
            lastChange = currentTime;
        }
        else if (glfwGetKey(window, GLFW_KEY_DELETE))
        {
            setText("", getFontSize(), getColor(), windowWidth, windowHeight);
            textCursorOffset = 0;
            lastChange = currentTime;
        }
        else if (gui_utility::charCodePoint != 0 && gui_utility::charCodePoint < 128)
        {
            std::string text = getText();
            if (textCursorOffset == 0)
                text += (char)gui_utility::charCodePoint;
            else
                text = text.substr(0, text.size() - textCursorOffset) + (char)gui_utility::charCodePoint + text.substr(text.size() - textCursorOffset);
            setText(text, getFontSize(), getColor(), windowWidth, windowHeight);
            lastChange = currentTime;
        }
    }
    return mouseDrag;
}