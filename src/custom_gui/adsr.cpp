#include "../../header/custom_gui/adsr.hpp"

customGUI::ADSR::ADSR(unsigned int textureWidth, unsigned int textureHeight, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;

    textureBuffer.resize(textureWidth * textureHeight * 3);

    texture = new Texture(textureBuffer.data(), textureWidth, textureHeight, 3, Texture::DIFFUSE, GL_NONE, GL_NONE, GL_NEAREST);
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, texture));
}
customGUI::ADSR::~ADSR()
{
    if (texture)
        delete texture;
}

void customGUI::ADSR::update(audio::ADSR const *adsrData)
{
    for (size_t i = 0; i < textureBuffer.size(); i += 3)
        textureBuffer[i] = textureBuffer[i + 1] = textureBuffer[i + 2] = 0;

    adsrData->draw(textureBuffer, textureWidth, textureHeight, 3);

    glBindTexture(GL_TEXTURE_2D, texture->getID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureBuffer.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void customGUI::ADSR::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}