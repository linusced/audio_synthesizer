#include "../../../header/gl/gui.hpp"

gui::native::Text::Text(Font *font, std::string text, int fontSize, glm::vec3 color, bool keepFontSize, ALIGNMENT_H textAlignment_horizontal, ALIGNMENT_V textAlignment_vertical, Constraints *constraints) : Element(constraints)
{
    this->font = font;
    this->keepFontSize = keepFontSize;
    this->text = text;
    this->fontSize = fontSize;
    this->color = color;
    this->alignment_h = textAlignment_horizontal;
    this->alignment_v = textAlignment_vertical;

    if (!keepFontSize)
        createData(text, fontSize, color);
}

void gui::native::Text::createData(std::string text, int fontSize, glm::vec3 color, int maxWidth, int maxHeight)
{
    if (keepFontSize)
    {
        maxWidth = abs(maxWidth);
        maxHeight = abs(maxHeight);
        maxWidth -= 15;
    }
    std::vector<std::string> lines;
    int lineCount = std::count(text.begin(), text.end(), '\n') + 1;
    for (int i = 0; i < lineCount; i++)
    {
        lines.push_back(text.substr(0, text.find('\n')));
        if (i + 1 < lineCount)
            text = text.substr(text.find('\n') + 1);
    }
    stbtt_fontinfo info;
    stbtt_InitFont(&info, font->buffer, 0);

    constexpr int imgSizeMult = 5;
    int lineHeight = (fontSize + 4) * imgSizeMult;
    float scale = stbtt_ScaleForPixelHeight(&info, fontSize * imgSizeMult);
    int x = 0, ascent, descent, lineGap, lineOffset = 0;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    if (keepFontSize)
    {
        for (int l = 0; l < lines.size(); l++)
        {
            x = 0;
            text = lines[l];
            for (int i = 0; i < text.size(); i++)
            {
                int ax;
                stbtt_GetCodepointHMetrics(&info, text[i], &ax, (int *)0);
                x += roundf(ax * scale);
                int kern = stbtt_GetCodepointKernAdvance(&info, text[i], text[i + 1]);
                x += roundf(kern * scale);
                if (x >= (maxWidth * imgSizeMult))
                {
                    size_t split = text.substr(0, i + 1).find_last_of(' ');
                    if (split == std::string::npos)
                        split = i + 1;
                    lines[l] = text.substr(0, split);
                    lines.insert(lines.begin() + l + 1, text.substr(split + 1));
                    break;
                }
            }
        }
        currentLines = lines.size();
        for (int l = 0; l < lines.size(); l++)
            if (lineHeight * (l + 1 - lineScroll) >= (maxHeight * imgSizeMult))
            {
                lines.erase(std::next(lines.begin(), l), lines.end());
                break;
            }
        if (lineScroll > 0)
            lines.erase(lines.begin(), std::next(lines.begin(), lineScroll));
        visibleLines = lines.size();
    }
    textHeight = lines.size() * lineHeight;
    textWidth = numOfChars = 0;
    if (lines.size() == 1)
    {
        numOfChars = lines[0].size();
        if (charWidth)
            delete charWidth;
        charWidth = (float *)calloc(numOfChars, sizeof(float));
    }
    int prevX{0};
    for (int l = 0; l < lines.size(); l++)
    {
        x = 0;
        text = lines[l];
        for (int i = 0; i < text.size(); i++)
        {
            int ax;
            stbtt_GetCodepointHMetrics(&info, text[i], &ax, (int *)0);
            x += roundf(ax * scale);
            int kern = stbtt_GetCodepointKernAdvance(&info, text[i], text[i + 1]);
            x += roundf(kern * scale);
            if (numOfChars > 0)
            {
                charWidth[i] = (float)(x - prevX);
                prevX = x;
            }
        }
        if (x > textWidth)
        {
            textWidth = x;
            for (int i = 0; i < numOfChars; i++)
                charWidth[i] /= (float)textWidth;
        }
    }
    unsigned long b_size = textWidth * textHeight;
    unsigned char *bitmap = (unsigned char *)calloc(b_size, sizeof(unsigned char));
    textData = (unsigned char *)calloc(b_size * 4, sizeof(unsigned char));

    for (int l = 0; l < lines.size(); l++)
    {
        x = 0;
        text = lines[l];
        for (int i = 0; i < text.size(); i++)
        {
            int ax, lsb;
            stbtt_GetCodepointHMetrics(&info, text[i], &ax, &lsb);
            int c_x1, c_y1, c_x2, c_y2;
            stbtt_GetCodepointBitmapBox(&info, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
            int y = ascent + c_y1 + lineOffset, byteOffset = x + roundf(lsb * scale) + (y * textWidth);
            stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, textWidth, scale, scale, text[i]);
            x += roundf(ax * scale);
            int kern = stbtt_GetCodepointKernAdvance(&info, text[i], text[i + 1]);
            x += roundf(kern * scale);
        }
        lineOffset += lineHeight;
    }
    for (unsigned long i = 0, j = 0; i < b_size; i++, j += 4)
    {
        textData[j] = bitmap[i] * color.r;
        textData[j + 1] = bitmap[i] * color.g;
        textData[j + 2] = bitmap[i] * color.b;
        textData[j + 3] = bitmap[i] == 255 ? 255 : 0;
    }
    delete bitmap;

    unsigned char *outputTextData = (unsigned char *)calloc((textWidth / imgSizeMult) * (textHeight / imgSizeMult) * 4, sizeof(unsigned char));

    stbir_resize_uint8(textData, textWidth, textHeight, 0, outputTextData, textWidth / imgSizeMult, textHeight / imgSizeMult, 0, 4);
    textWidth /= imgSizeMult;
    textHeight /= imgSizeMult;

    if (poly.size() > 0)
    {
        delete poly[0];
        poly[0] = new Polygon({Vertex2D({0, 0}, {0, 1}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {0, 0})}, {0, 1, 2, 0, 2, 3}, new Texture(outputTextData, textWidth, textHeight, 4, Texture::DIFFUSE));
    }
    else
    {
        poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 1}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {0, 0})}, {0, 1, 2, 0, 2, 3}, new Texture(outputTextData, textWidth, textHeight, 4, Texture::DIFFUSE)));
    }
    if (keepFontSize && poly.size() < 2)
    {
        std::vector<Vertex2D> vertices;
        for (int i = 0; i < 8; i++)
            vertices.push_back({{0, 0}, scrollThumbColor});
        poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7}));
    }
}

void gui::native::Text::setText(std::string text, int fontSize, glm::vec3 color, int &windowWidth, int &windowHeight)
{
    this->text = text;
    this->fontSize = fontSize;
    this->color = color;
    if (!keepFontSize)
        createData(text, fontSize, color);
    resizePoly(windowWidth, windowHeight);
}
std::string gui::native::Text::getText()
{
    return text;
}
int gui::native::Text::getFontSize()
{
    return fontSize;
}
glm::vec3 gui::native::Text::getColor()
{
    return color;
}
int gui::native::Text::getTextWidth()
{
    return textWidth * textSizeMult;
}
int gui::native::Text::getTextHeight()
{
    return textHeight * textSizeMult;
}
float gui::native::Text::getCharWidth(int index)
{
    if (index >= 0 && index < numOfChars)
        return charWidth[index];
    return 0;
}

void gui::native::Text::setScrollSliderColors(glm::vec3 trackColor, glm::vec3 thumbColor)
{
    scrollTrackColor = trackColor;
    scrollThumbColor = thumbColor;
}

void gui::native::Text::resizePoly(int &windowWidth, int &windowHeight)
{
    if (!scrollResize)
        lineScroll = 0;
    scrollResize = false;
    float layout_w_px = layout->w * windowWidth, layout_h_px = layout->h * windowHeight;
    if (keepFontSize)
        createData(this->text, this->fontSize, this->color, layout_w_px, layout_h_px);
    float w = layout->w * (textWidth / layout_w_px), h = layout->h * (textHeight / layout_h_px),
          w_mult = abs(layout->w / w), h_mult = abs(layout->h / h), min_mult = w_mult < h_mult ? w_mult : h_mult;
    if (min_mult < 1 && !keepFontSize)
    {
        w *= min_mult;
        h *= min_mult;
        textSizeMult = min_mult;
    }
    else
        textSizeMult = 1;

    float x = layout->x, y = layout->y;

    if (alignment_h == ALIGNMENT_CENTER_H)
        x += ((layout->w - w) / 2);
    else if (alignment_h == ALIGNMENT_RIGHT_H)
        x = (layout->x + layout->w) - w;

    if (alignment_v == ALIGNMENT_CENTER_V)
        y += ((layout->h - h) / 2);
    else if (alignment_v == ALIGNMENT_BOTTOM_V)
        y = (layout->y + layout->h) - h;

    poly[0]->vertices[0].position = {x, y};
    poly[0]->vertices[1].position = {x + w, y};
    poly[0]->vertices[2].position = {x + w, y + h};
    poly[0]->vertices[3].position = {x, y + h};

    poly[0]->applyVertexChange();

    if (keepFontSize)
    {
        float scrollW = 12.0 / windowWidth;

        poly[1]->vertices[0].position = {layout->x + layout->w - scrollW, layout->y};
        poly[1]->vertices[1].position = {layout->x + layout->w, layout->y};
        poly[1]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
        poly[1]->vertices[3].position = {layout->x + layout->w - scrollW, layout->y + layout->h};

        for (int i = 0; i < 4; i++)
            poly[1]->vertices[i].color = glm::vec4(scrollTrackColor, currentLines == visibleLines ? 0 : 1);

        float scrollY = layout->y + (layout->h / currentLines) * lineScroll, scrollH = (layout->h / currentLines) * visibleLines;
        if (currentLines == visibleLines)
            scrollH = 0;
        poly[1]->vertices[4].position = {layout->x + layout->w - scrollW, scrollY};
        poly[1]->vertices[5].position = {layout->x + layout->w, scrollY};
        poly[1]->vertices[6].position = {layout->x + layout->w, scrollY + scrollH};
        poly[1]->vertices[7].position = {layout->x + layout->w - scrollW, scrollY + scrollH};

        poly[1]->applyVertexChange();
    }
}
void gui::native::Text::scrollPoly(int &windowWidth, int &windowHeight)
{
    scrollResize = true;
    resizePoly(windowWidth, windowHeight);
}
bool gui::native::Text::inputs(GLFWwindow *window, int relativeMousePos_x, int relativeMousePos_y, int &windowWidth, int &windowHeight)
{
    bool lmbDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (keepFontSize)
    {
        float width = layout->w * windowWidth, height = layout->h * windowHeight;
        if (!lmbDown)
        {
            scrollDragPrevY = -1;
            mouseDrag = false;
            for (int i = 4; i < 8; i++)
                poly[1]->vertices[i].color = glm::vec4(scrollThumbColor, 1);
            poly[1]->applyVertexChange();
            draw();
        }
        if ((lmbDown && (relativeMousePos_x >= width - 12 || scrollDragPrevY > -1)))
        {
            mouseDrag = true;
            if (scrollDragPrevY > -1)
            {
                int prevLineScroll = lineScroll;
                lineScroll += (int)((relativeMousePos_y - scrollDragPrevY) / (height / currentLines));
                if (lineScroll != prevLineScroll)
                    scrollDragPrevY = relativeMousePos_y;

                if (lineScroll < 0 || currentLines == visibleLines)
                    lineScroll = 0;
                else if (lineScroll >= currentLines - visibleLines)
                    lineScroll = currentLines - visibleLines;

                if (lineScroll != prevLineScroll)
                    scrollPoly(windowWidth, windowHeight);
            }
            else
            {
                scrollDragPrevY = relativeMousePos_y;
                for (int i = 4; i < 8; i++)
                    poly[1]->vertices[i].color = glm::vec4(scrollThumbColor, .8);
                poly[1]->applyVertexChange();
                draw();
            }
        }
        else if (gui_utility::scrollY != 0)
        {
            lineScroll -= gui_utility::scrollY;
            if (lineScroll < 0 || currentLines == visibleLines)
                lineScroll = 0;
            else if (lineScroll >= currentLines - visibleLines)
                lineScroll = currentLines - visibleLines;
            scrollPoly(windowWidth, windowHeight);
            return true;
        }
    }
    return lmbDown;
}