#include "../../header/custom_gui/waveform_time.hpp"

customGUI::WaveformTime::WaveformTime(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;

    textureBuffer.resize(textureWidth * textureHeight * 3);

    texture = new Texture(textureBuffer.data(), textureWidth, textureHeight, 3, Texture::DIFFUSE, GL_NONE, GL_NONE, GL_LINEAR);
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, texture));
}
customGUI::WaveformTime::~WaveformTime()
{
    if (texture)
        delete texture;
}

void customGUI::WaveformTime::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}

void customGUI::WaveformTime::update(const std::vector<double> &timeDomain)
{
    for (size_t i = 0; i < textureBuffer.size(); i += 3)
        textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 0;

    auto fillRect = [&](int x, int y, int w, int h)
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
                    textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 255;
            }
    };

    int step = timeDomain.size() / textureWidth;
    for (int x = 0; x < textureWidth; x++)
    {
        double min = 1, max = -1;

        for (int y = 0; y < step; y++)
        {
            double value = timeDomain[(x * step) + y];
            if (value < min)
                min = value;
            if (value > max)
                max = value;
        }

        fillRect(x, (1 + min) * (textureHeight / 2), 1, (max - min) * (textureHeight / 2));
    }

    glBindTexture(GL_TEXTURE_2D, texture->getID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}