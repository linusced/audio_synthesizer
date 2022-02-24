#include "../../header/custom_gui/keyboard_midi.hpp"

customGUI::KeyboardMIDI::KeyboardMIDI(unsigned int textureWidth, unsigned int textureHeight, std::vector<short> *pressedMIDI, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;
    this->pressedMIDI = pressedMIDI;

    textureBuffer.resize(textureWidth * textureHeight * 3);

    texture = new Texture(textureBuffer.data(), textureWidth, textureHeight, 3, Texture::DIFFUSE, GL_NONE, GL_NONE, GL_LINEAR);
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, texture));

    double whiteKeys = 8 * 7 + 1;
    for (short i = 0, id = 27; i < whiteKeys; i++)
    {
        keysMIDI.push_back({id, (int)((i / whiteKeys) * textureWidth) + 2, 0, (int)((1 / whiteKeys) * textureWidth) - 4, (int)textureHeight, false});

        if (i % 7 == 6 || i % 7 == 2)
            id++;
        else
            id += 2;
    }

    for (short i = 0, id = 28; i < whiteKeys - 1; i++)
    {
        if (i % 7 == 6 || i % 7 == 2)
        {
            id++;
            continue;
        }

        keysMIDI.push_back({id, (int)((i / whiteKeys + .7 / whiteKeys) * textureWidth) + 2, (int)(textureHeight * .3), (int)((.8 / whiteKeys) * textureWidth) - 2, (int)(textureHeight * .7), true});

        id += 2;
    }
}
customGUI::KeyboardMIDI::~KeyboardMIDI()
{
    if (texture)
        delete texture;
}

void customGUI::KeyboardMIDI::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}

void customGUI::KeyboardMIDI::draw()
{
    for (size_t i = 0; i < textureBuffer.size(); i += 3)
        textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 0;

    auto fillRect = [&](int x, int y, int w, int h, unsigned char r, unsigned char g, unsigned char b)
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
                    textureBuffer[i] = r;
                    textureBuffer[i + 1] = g;
                    textureBuffer[i + 2] = b;
                }
            }
    };

    unsigned char r, g, b;
    for (auto &k : keysMIDI)
    {
        r = g = b = k.isBlack ? 60 : 255;

        auto it = std::find(pressedMIDI->begin(), pressedMIDI->end(), k.id);
        if (it != pressedMIDI->end())
            if (k.isBlack)
            {
                r = 255;
                g = 0;
                b = 80;
            }
            else
            {
                r = 0;
                g = 127;
                b = 255;
            }

        fillRect(k.x, k.y, k.w, k.h, r, g, b);
    }

    glBindTexture(GL_TEXTURE_2D, texture->getID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    poly[0]->draw();

    for (Element *ch : children)
        if (ch->draw_b)
            ch->draw();
}