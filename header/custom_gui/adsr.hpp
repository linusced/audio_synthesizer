#ifndef CUSTOM_GUI_ADSR
#define CUSTOM_GUI_ADSR

#include "../gl/gui.hpp"
#include "../audio/audio_adsr.hpp"

namespace customGUI
{
    class ADSR : public gui::Element
    {
    private:
        unsigned int textureWidth, textureHeight;

        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;

    public:
        ADSR(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints);
        ~ADSR();

        void update(audio::ADSR const *adsrData);

        void resizePoly(int &windowWidth, int &windowHeight) override;
    };
}

#endif