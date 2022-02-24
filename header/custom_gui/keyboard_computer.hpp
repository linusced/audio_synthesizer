#ifndef CUSTOM_GUI_KEYBOARD_COMPUTER
#define CUSTOM_GUI_KEYBOARD_COMPUTER

#include "../gl/gui.hpp"

namespace customGUI
{
    class KeyboardComputer : public gui::Element
    {
    private:
        struct Key
        {
            char keyChar;
            GLenum keyEnum;
            int x, y, w, h;
            bool isBlack;
        };

        unsigned int textureWidth, textureHeight;
        GLFWwindow *window = nullptr;

        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;
        std::vector<Key> computerKeys;
        std::vector<gui::Constraints *> keyLabelConstraints;

    public:
        KeyboardComputer(unsigned int textureWidth, unsigned int textureHeight, GLFWwindow *window, gui::Font *font, gui::Constraints *constraints);
        ~KeyboardComputer();

        void resizePoly(int &windowWidth, int &windowHeight) override;
        void draw() override;
    };
}

#endif