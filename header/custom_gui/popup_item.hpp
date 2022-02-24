#ifndef CUSTOM_GUI_POPUP_ITEM
#define CUSTOM_GUI_POPUP_ITEM

#include "../gl/gui.hpp"

namespace customGUI
{
    class PopupItem : public gui::Element
    {
    private:
        int index;
        std::function<void(int)> *callback = nullptr;
        glm::vec4 bgColor, clickColor;
        gui::native::Text *textElement = nullptr;

        void changeColor(int color);

    public:
        PopupItem(std::string text, int index, std::function<void(int)> *callback, glm::vec4 bgColor, glm::vec4 clickColor, glm::vec3 textColor, gui::Font *font, gui::Constraints *btnText, gui::Constraints *constraints, bool centerText = false);

        void resizePoly(int &windowWidth, int &windowHeight) override;
        bool inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight) override;
    };
}

#endif