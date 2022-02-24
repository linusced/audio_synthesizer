#ifndef AUDIO_WAVEFORM
#define AUDIO_WAVEFORM

#include "audio_common.hpp"

namespace audio
{
    struct Harmonic
    {
        double amplitude = 0, phase = 0;
    };

    struct Waveform
    {
        double amplitude = .2;
        std::vector<Harmonic> harmonics{{1, 0}};

        void fillBuffer(std::vector<int16_t> &buffer, double frequency, double timeOffset, int sampleRate) const;
        void draw(std::vector<uint8_t> &textureBuffer, int width, int height, int colorChannels) const;
    };
}

#endif