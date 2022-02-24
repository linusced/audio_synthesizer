#include "../../header/audio/audio_adsr.hpp"

void audio::ADSR::modifyBuffer(std::vector<int16_t> &buffer, double timeOffset, double startTime, double endTime, int sampleRate) const
{
    if (endTime == -1)
        endTime = 10000;
    else
        endTime -= startTime;

    for (size_t i = 0; i < buffer.size(); i++)
    {
        double time = (timeOffset - startTime) + i / (double)sampleRate;
        if (time < attack_time && time < endTime)
        {
            buffer[i] *= time / attack_time;
        }
        else if (time < attack_time + decay_time && time < endTime)
        {
            time -= attack_time;
            buffer[i] *= 1 - (time / decay_time) * (1 - sustain_value);
        }
        else if (time > endTime)
        {
            time -= endTime;
            if (time < release_time)
                buffer[i] *= sustain_value - (time / release_time) * sustain_value;
            else
                buffer[i] = 0;
        }
        else
            buffer[i] *= sustain_value;
    }
}

void audio::ADSR::draw(std::vector<uint8_t> &textureBuffer, int width, int height, int colorChannels) const
{
    auto fillRect = [&](int x, int y, int w, int h, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255)
    {
        if (w < 1)
            w = 1;
        if (h < 1)
            h = 1;

        int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
        for (x = x1; x < x2; x++)
            for (y = y1; y < y2; y++)
            {
                size_t i = (y * (size_t)width + x) * 3UL;
                if (i + 2 < textureBuffer.size())
                {
                    textureBuffer[i] = r;
                    if (colorChannels > 1)
                        textureBuffer[i + 1] = g;
                    if (colorChannels > 2)
                        textureBuffer[i + 2] = b;
                    if (colorChannels > 3)
                        textureBuffer[i + 3] = 255;
                }
            }
    };

    double time = attack_time + decay_time + release_time,
           sustain_time = time / 3.0;
    time += sustain_time;

    for (int x = 0; x < width; x++)
    {
        double x_time = (x / (double)width) * time;

        if (x_time < attack_time)
        {
            fillRect(x, 0, 1, (x_time / attack_time) * height);
        }
        else if (x_time < attack_time + decay_time)
        {
            x_time -= attack_time;
            fillRect(x, 0, 1, (1 - (x_time / decay_time) * (1 - sustain_value)) * height);
        }
        else if (x_time < attack_time + decay_time + sustain_time)
        {
            fillRect(x, 0, 1, sustain_value * height);
        }
        else
        {
            x_time -= attack_time + decay_time + sustain_time;
            fillRect(x, 0, 1, (sustain_value - (x_time / release_time) * sustain_value) * height);
        }
    }

    fillRect((attack_time / time) * width - 2, 0, 4, height, 0, 127, 255);
    fillRect(((attack_time + decay_time) / time) * width - 2, 0, 4, sustain_value * height, 0, 127, 255);
    fillRect(((attack_time + decay_time + sustain_time) / time) * width - 2, 0, 4, sustain_value * height, 0, 127, 255);
}