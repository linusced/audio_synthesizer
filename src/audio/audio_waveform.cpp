#include "../../header/audio/audio_waveform.hpp"

void audio::Waveform::fillBuffer(std::vector<int16_t> &buffer, double frequency, double timeOffset, int sampleRate) const
{
    std::vector<double> data(buffer.size());
    double max = 0, time;
    size_t i, h;

    for (i = 0; i < buffer.size(); i++)
    {
        time = timeOffset + i / (double)sampleRate;

        for (h = 0; h < harmonics.size(); h++)
            if (harmonics[h].amplitude > 0)
                data[i] += harmonics[h].amplitude * sin(2 * M_PI * (frequency * (h + 1)) * time + harmonics[h].phase);

        if (abs(data[i]) > max)
            max = abs(data[i]);
    }

    double mult = (amplitude / max) * ((pow(2, 16) - 1) / 2);

    for (i = 0; i < buffer.size(); i++)
        buffer[i] += data[i] * mult;
}

void audio::Waveform::draw(std::vector<uint8_t> &textureBuffer, int width, int height, int colorChannels) const
{
    auto drawPixel = [&](int x, int y)
    {
        size_t i = (y * (size_t)width + x) * (size_t)colorChannels;

        textureBuffer[i] = 255;
        if (colorChannels > 1)
            textureBuffer[i + 1] = 255;
        if (colorChannels > 2)
            textureBuffer[i + 2] = 255;
        if (colorChannels > 3)
            textureBuffer[i + 3] = 255;
    };

    std::vector<double> data(width);
    double max = 0;

    for (int x = 0; x < width; x++)
    {
        double time = (x / ((double)width - 1.0));

        data[x] = 0;
        for (int h = 0; h < harmonics.size(); h++)
            data[x] += harmonics[h].amplitude * sin(2 * M_PI * (h + 1) * time + harmonics[h].phase);

        if (abs(data[x]) > max)
            max = abs(data[x]);
    }
    if (max == 0)
        max = 1;

    double mult = (1 / max) * ((height - 1) / 2);
    int prevY = -1;

    for (int x = 0; x < width; x++)
    {
        int y = data[x] * mult + (height / 2);
        if (y == prevY || prevY == -1)
        {
            y += (y < height / 2) ? 2 : -2;
            prevY = y;
        }

        bool greaterThan = y >= prevY;
        for (int y2 = y; (greaterThan ? (y2 >= prevY) : (y2 <= prevY)); y2 += (greaterThan ? -1 : 1))
        {
            drawPixel(x, y2);
            if (x > 0)
                drawPixel(x - 1, y2);
            if (x < width - 1)
                drawPixel(x + 1, y2);
        }

        prevY = y;
    }
}