#ifndef CUSTOM_GUI_KEYBOARD_MIDI
#define CUSTOM_GUI_KEYBOARD_MIDI

#include "../gl/gui.hpp"

namespace customGUI
{
    class KeyboardMIDI : public gui::Element
    {
    private:
        struct Key
        {
            short id;
            int x, y, w, h;
            bool isBlack;
        };

        unsigned int textureWidth, textureHeight;
        std::vector<short> *pressedMIDI = nullptr;

        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;
        std::vector<Key> keysMIDI;

    public:
        KeyboardMIDI(unsigned int textureWidth, unsigned int textureHeight, std::vector<short> *pressedMIDI, gui::Constraints *constraints);
        ~KeyboardMIDI();

        void resizePoly(int &windowWidth, int &windowHeight) override;
        void draw() override;
    };
}

#endif