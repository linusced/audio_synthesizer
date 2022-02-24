#ifndef AUDIO_ADSR
#define AUDIO_ADSR

#include "audio_common.hpp"

namespace audio
{
    struct ADSR
    {
        double attack_time = .01, decay_time = 0, sustain_value = 1, release_time = .01;

        void modifyBuffer(std::vector<int16_t> &buffer, double timeOffset, double startTime, double endTime, int sampleRate) const;
        void draw(std::vector<uint8_t> &textureBuffer, int width, int height, int colorChannels) const;
    };
}

#endif