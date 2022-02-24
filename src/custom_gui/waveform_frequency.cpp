#include "../../header/custom_gui/waveform_frequency.hpp"

customGUI::WaveformFrequency::WaveformFrequency(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;

    textureBuffer.resize(textureWidth * textureHeight * 3);

    texture = new Texture(textureBuffer.data(), textureWidth, textureHeight, 3, Texture::DIFFUSE, GL_NONE, GL_NONE, GL_LINEAR);
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, texture));
}
customGUI::WaveformFrequency::~WaveformFrequency()
{
    if (texture)
        delete texture;
}

void customGUI::WaveformFrequency::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}

void customGUI::WaveformFrequency::update(const std::vector<double> &frequencyDomain, const std::vector<std::vector<int>> &frequencyDistribution, unsigned int sampleRate, bool linearOverride)
{
    for (size_t i = 0; i < textureBuffer.size(); i += 3)
        textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 0;

    auto fillRect = [&](int x, int y, int w, int h, bool color = false)
    {
        if (w < 1)
            w = 1;
        if (h < 1)
            h = 1;

        int x1 = x, y1 = y, x2 = x + w, y2 = y + h;
        for (x = x1; x < x2; x++)
            for (y = y1; y < y2; y++)
            {
                size_t i = (y * (size_t)textureWidth + x) * 3UL;
                if (i + 2 < textureBuffer.size())
                {
                    if (color)
                    {
                        textureBuffer[i] = 255;
                        textureBuffer[i + 1] = 127;
                        textureBuffer[i + 2] = 0;
                    }
                    else
                        textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 255;
                }
            }
    };

    if (linearOverride)
    {
        int step = frequencyDomain.size() / textureWidth;
        for (int x = 0; x < textureWidth; x++)
            fillRect(x, 0, 1, (frequencyDomain[x * step] / 60.0) * textureHeight);
    }
    else
    {
        auto getFrequencyForWidth = [&](int x, int width, int prevWidth, int width_hz, int prevWidth_hz)
        {
            double percentage = (x / (double)(width - prevWidth)) - (prevWidth / (double)(width - prevWidth));
            return (int)((percentage * (width_hz - prevWidth_hz)) + prevWidth_hz);
        };
        auto getIndexForFrequency = [&](int hz)
        {
            return (int)(hz * ((frequencyDomain.size() * 2.0) / (sampleRate / 2.0)));
        };
        auto getMaxValue = [&](int prevIndex, int index)
        {
            double max = 0;
            for (int i = prevIndex; i <= index; i++)
                if (i < frequencyDomain.size() && frequencyDomain[i] > max)
                    max = frequencyDomain[i];
            return max;
        };

        double value;
        int x = 0, width = 0, prevWidth = 0, width_hz = 0, prevWidth_hz = 0, index = 0, prevIndex = -1;
        for (int i = 0; i < frequencyDistribution.size(); i++)
        {
            width_hz = frequencyDistribution[i][0];
            width = (frequencyDistribution[i][1] / 1000.0) * textureWidth;
            if (width == textureWidth)
                width--;

            for (x; x <= width; x++)
            {
                index = getIndexForFrequency(getFrequencyForWidth(x, width, prevWidth, width_hz, prevWidth_hz));
                if (prevIndex == -1)
                    value = frequencyDomain[index];
                else
                    value = getMaxValue(prevIndex, index);

                fillRect(x, 0, 1, (value / 60.0) * textureHeight);
                prevIndex = index;
            }
            prevWidth = width;
            prevWidth_hz = width_hz;
        }
    }

    fillRect(0, 0, textureWidth, 4, true);
    fillRect(0, textureHeight - 4, textureWidth, 4, true);

    glBindTexture(GL_TEXTURE_2D, texture->getID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}