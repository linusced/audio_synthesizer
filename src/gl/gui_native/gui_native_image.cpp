#include "../../../header/gl/gui.hpp"

gui::native::Image::Image(std::string filePath, bool scaleToCoverRect, ALIGNMENT_H imgAlignment_horizontal, ALIGNMENT_V imgAlignment_vertical, Constraints *constraints) : Element(constraints)
{
    this->scaleToCoverRect = scaleToCoverRect;
    alignment_h = imgAlignment_horizontal;
    alignment_v = imgAlignment_vertical;

    updateImage(filePath);
}
gui::native::Image::~Image()
{
    if (t)
        delete t;
}

void gui::native::Image::updateImage(std::string filePath)
{
    if (t)
        delete t;

    t = new Texture(filePath, Texture::DIFFUSE);
    imgWidth = t->getWidth();
    imgHeight = t->getHeight();

    for (Polygon *p : poly)
        delete p;
    poly.clear();
    poly.push_back(new Polygon({Vertex2D({0, 0}, {0, 0}), Vertex2D({0, 0}, {1, 0}), Vertex2D({0, 0}, {1, 1}), Vertex2D({0, 0}, {0, 1})}, {0, 1, 2, 0, 2, 3}, t));
}

void gui::native::Image::resizePoly(int &windowWidth, int &windowHeight)
{
    float layout_w_px = layout->w * windowWidth, layout_h_px = layout->h * windowHeight;
    float x = layout->x, y = layout->y, w = layout->w, h = layout->h;
    if (scaleToCoverRect)
    {
        if ((imgWidth / layout_w_px) < (imgHeight / layout_h_px))
        {
            poly[0]->vertices[0].texture = {0, 1};
            poly[0]->vertices[1].texture = {1, 1};
            poly[0]->vertices[2].texture = {1, 0};
            poly[0]->vertices[3].texture = {0, 0};

            float diff = (layout_h_px / layout_w_px) / ((float)imgHeight / (float)imgWidth);
            if (alignment_v == ALIGNMENT_TOP_V)
            {
                poly[0]->vertices[2].texture.y += diff * 2;
                poly[0]->vertices[3].texture.y += diff * 2;
            }
            else if (alignment_v == ALIGNMENT_CENTER_V)
            {
                poly[0]->vertices[0].texture.y -= diff;
                poly[0]->vertices[1].texture.y -= diff;
                poly[0]->vertices[2].texture.y += diff;
                poly[0]->vertices[3].texture.y += diff;
            }
            else if (alignment_v == ALIGNMENT_BOTTOM_V)
            {
                poly[0]->vertices[0].texture.y -= diff * 2;
                poly[0]->vertices[1].texture.y -= diff * 2;
            }
        }
        else
        {
            poly[0]->vertices[0].texture = {1, 0};
            poly[0]->vertices[1].texture = {0, 0};
            poly[0]->vertices[2].texture = {0, 1};
            poly[0]->vertices[3].texture = {1, 1};

            float diff = (layout_w_px / layout_h_px) / ((float)imgWidth / (float)imgHeight);
            if (alignment_h == ALIGNMENT_LEFT_H)
            {
                poly[0]->vertices[1].texture.x += diff * 2;
                poly[0]->vertices[2].texture.x += diff * 2;
            }
            else if (alignment_h == ALIGNMENT_CENTER_H)
            {
                poly[0]->vertices[0].texture.x -= diff;
                poly[0]->vertices[1].texture.x += diff;
                poly[0]->vertices[2].texture.x += diff;
                poly[0]->vertices[3].texture.x -= diff;
            }
            else if (alignment_h == ALIGNMENT_RIGHT_H)
            {
                poly[0]->vertices[0].texture.x -= diff * 2;
                poly[0]->vertices[3].texture.x -= diff * 2;
            }
        }
    }
    else
    {
        w *= (imgWidth / layout_w_px), h *= (imgHeight / layout_h_px);
        float w_mult = abs(layout->w / w), h_mult = abs(layout->h / h), min_mult = w_mult < h_mult ? w_mult : h_mult;
        if (min_mult < 1)
        {
            w *= min_mult;
            h *= min_mult;
        }
        if (alignment_h == ALIGNMENT_CENTER_H)
            x += ((layout->w - w) / 2);
        else if (alignment_h == ALIGNMENT_RIGHT_H)
            x = (layout->x + layout->w) - w;

        if (alignment_v == ALIGNMENT_CENTER_V)
            y += ((layout->h - h) / 2);
        else if (alignment_v == ALIGNMENT_BOTTOM_V)
            y = (layout->y + layout->h) - h;
    }

    poly[0]->vertices[0].position = {x, y};
    poly[0]->vertices[1].position = {x + w, y};
    poly[0]->vertices[2].position = {x + w, y + h};
    poly[0]->vertices[3].position = {x, y + h};

    poly[0]->applyVertexChange();
}