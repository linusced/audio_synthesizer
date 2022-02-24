#ifndef CUSTOM_GUI_WAVEFORM
#define CUSTOM_GUI_WAVEFORM

#include "../gl/gui.hpp"
#include "../audio/audio_waveform.hpp"

namespace customGUI
{
    class Waveform : public gui::Element
    {
    private:
        unsigned int textureWidth, textureHeight;

        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;

    public:
        Waveform(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints);
        ~Waveform();

        void update(audio::Waveform const *waveData);

        void resizePoly(int &windowWidth, int &windowHeight) override;
    };
}

#endif