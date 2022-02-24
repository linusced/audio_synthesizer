#ifndef CUSTOM_GUI_WAVEFORM_TIME
#define CUSTOM_GUI_WAVEFORM_TIME

#include "../gl/gui.hpp"

namespace customGUI
{
    class WaveformTime : public gui::Element
    {
    private:
        unsigned int textureWidth, textureHeight;
        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;

    public:
        WaveformTime(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints);
        ~WaveformTime();

        void resizePoly(int &windowWidth, int &windowHeight) override;

        void update(const std::vector<double> &timeDomain);
    };
}

#endif