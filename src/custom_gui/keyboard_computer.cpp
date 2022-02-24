#include "../../header/custom_gui/keyboard_computer.hpp"

customGUI::KeyboardComputer::KeyboardComputer(unsigned int textureWidth, unsigned int textureHeight, GLFWwindow *window, gui::Font *font, gui::Constraints *constraints)
    : gui::Element(constraints)
{
    this->textureWidth = textureWidth;
    this->textureHeight = textureHeight;
    this->window = window;

    textureBuffer.resize(textureWidth * textureHeight * 3);

    texture = new Texture(textureBuffer.data(), textureWidth, textureHeight, 3, Texture::DIFFUSE, GL_NONE, GL_NONE, GL_LINEAR);
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, texture));

    std::string keyCharacters = "ASDFGHJKL";
    std::vector<GLenum> keyCharactersEnum = {GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L};

    double whiteKeys = 10;
    for (short i = 0; i < whiteKeys - 1; i++)
    {
        computerKeys.push_back({keyCharacters[i], keyCharactersEnum[i], (int)((i / whiteKeys) * textureWidth) + 2, 0, (int)((1 / whiteKeys) * textureWidth) - 4, (int)(textureHeight * .47), false});

        keyLabelConstraints.push_back(new gui::Constraints(gui::RELATIVE_CONSTRAINT, computerKeys.back().x / (double)textureWidth, gui::RELATIVE_CONSTRAINT, .5 - (computerKeys.back().y / (double)textureHeight), gui::RELATIVE_CONSTRAINT, computerKeys.back().w / (double)textureWidth, gui::RELATIVE_CONSTRAINT, computerKeys.back().h / (double)textureHeight));
        addChild(new gui::native::Text(font, std::string(1, computerKeys.back().keyChar), 30, glm::vec3(0), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, keyLabelConstraints.back()));
    }

    keyCharacters = "WE TYU OP";
    keyCharactersEnum = {GLFW_KEY_W, GLFW_KEY_E, 0, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, 0, GLFW_KEY_O, GLFW_KEY_P};

    for (short i = 0; i < whiteKeys - 1; i++)
    {
        if (i % 7 == 6 || i % 7 == 2)
            continue;

        computerKeys.push_back({keyCharacters[i], keyCharactersEnum[i], (int)((i / whiteKeys + .4 / whiteKeys) * textureWidth) + 2, (int)(textureHeight * .53), (int)((1 / whiteKeys) * textureWidth) - 2, (int)(textureHeight * .47), true});

        keyLabelConstraints.push_back(new gui::Constraints(gui::RELATIVE_CONSTRAINT, computerKeys.back().x / (double)textureWidth, gui::RELATIVE_CONSTRAINT, .5 - (computerKeys.back().y / (double)textureHeight), gui::RELATIVE_CONSTRAINT, computerKeys.back().w / (double)textureWidth, gui::RELATIVE_CONSTRAINT, computerKeys.back().h / (double)textureHeight));
        addChild(new gui::native::Text(font, std::string(1, computerKeys.back().keyChar), 30, glm::vec3(0), false, gui::ALIGNMENT_CENTER_H, gui::ALIGNMENT_CENTER_V, keyLabelConstraints.back()));
    }
}
customGUI::KeyboardComputer::~KeyboardComputer()
{
    if (texture)
        delete texture;

    for (auto i : keyLabelConstraints)
        delete i;
}

void customGUI::KeyboardComputer::resizePoly(int &windowWidth, int &windowHeight)
{
    poly[0]->vertices[0].position = {layout->x, layout->y};
    poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
    poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
    poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

    poly[0]->applyVertexChange();
}

void customGUI::KeyboardComputer::draw()
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
    for (auto &k : computerKeys)
    {
        r = g = b = k.isBlack ? 150 : 255;

        if (glfwGetKey(window, k.keyEnum))
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