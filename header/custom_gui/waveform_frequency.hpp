#ifndef CUSTOM_GUI_WAVEFORM_FREQUENCY
#define CUSTOM_GUI_WAVEFORM_FREQUENCY

#include "../gl/gui.hpp"

namespace customGUI
{
    class WaveformFrequency : public gui::Element
    {
    private:
        unsigned int textureWidth, textureHeight;
        std::vector<uint8_t> textureBuffer;
        Texture *texture = nullptr;

    public:
        WaveformFrequency(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints);
        ~WaveformFrequency();

        void resizePoly(int &windowWidth, int &windowHeight) override;

        void update(const std::vector<double> &frequencyDomain, const std::vector<std::vector<int>> &frequencyDistribution, unsigned int sampleRate, bool linearOverride);
    };
}

#endif