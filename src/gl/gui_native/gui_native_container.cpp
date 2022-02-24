#include "../../../header/gl/gui.hpp"

gui::native::Container::Container(glm::vec4 backgroundColor, int borderRadius, Constraints *constraints) : Element(constraints)
{
    this->borderRadius = borderRadius;
    std::vector<Vertex2D> vertices;

    for (int i = 0; i < 4; i++)
        vertices.push_back({{0, 0}, backgroundColor});
    if (borderRadius == 0)
        poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));
    else
    {
        for (int i = 0; i < 20; i++)
            vertices.push_back({{0, 0}, backgroundColor});
        poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 1, 12, 4, 12, 13, 4, 13, 14, 4, 14, 5, 4, 6, 15, 7, 15, 16, 7, 16, 17, 7, 17, 2, 7, 3, 18, 10, 18, 19, 10, 19, 20, 10, 20, 11, 10, 8, 21, 9, 21, 22, 9, 22, 23, 9, 23, 0, 9}));
    }
}
gui::native::Container::Container(glm::vec4 backgroundColor, int borderWidth, glm::vec4 borderColor, Constraints *constraints) : Element(constraints)
{
    this->borderWidth = borderWidth;
    std::vector<Vertex2D> vertices;

    for (int i = 0; i < 4; i++)
        vertices.push_back({{0, 0}, backgroundColor});
    poly.push_back(new Polygon(vertices, {0, 1, 2, 0, 2, 3}));
    vertices.clear();
    for (int i = 0; i < 12; i++)
        vertices.push_back({{0, 0}, borderColor});
    poly.push_back(new Polygon(vertices, {0, 4, 5, 0, 5, 3, 6, 1, 2, 6, 2, 7, 4, 6, 8, 4, 8, 9, 5, 10, 11, 5, 11, 7}));
}

void gui::native::Container::resizePoly(int &windowWidth, int &windowHeight)
{
    if (borderRadius == 0)
    {
        poly[0]->vertices[0].position = {layout->x, layout->y};
        poly[0]->vertices[1].position = {layout->x + layout->w, layout->y};
        poly[0]->vertices[2].position = {layout->x + layout->w, layout->y + layout->h};
        poly[0]->vertices[3].position = {layout->x, layout->y + layout->h};

        poly[0]->applyVertexChange();

        if (borderWidth > 0)
        {
            float w_x = (float)borderWidth / windowWidth, w_y = (float)borderWidth / windowHeight;

            poly[1]->vertices[0].position = poly[0]->vertices[0].position;
            poly[1]->vertices[1].position = poly[0]->vertices[1].position;
            poly[1]->vertices[2].position = poly[0]->vertices[2].position;
            poly[1]->vertices[3].position = poly[0]->vertices[3].position;

            poly[1]->vertices[4].position = {layout->x + w_x, layout->y};
            poly[1]->vertices[5].position = {layout->x + w_x, layout->y + layout->h};
            poly[1]->vertices[6].position = {layout->x + layout->w - w_x, layout->y};
            poly[1]->vertices[7].position = {layout->x + layout->w - w_x, layout->y + layout->h};
            poly[1]->vertices[8].position = {layout->x + layout->w - w_x, layout->y + w_y};
            poly[1]->vertices[9].position = {layout->x + w_x, layout->y + w_y};
            poly[1]->vertices[10].position = {layout->x + w_x, layout->y + layout->h - w_y};
            poly[1]->vertices[11].position = {layout->x + layout->w - w_x, layout->y + layout->h - w_y};

            poly[1]->applyVertexChange();
        }
    }
    else
    {
        float r_x = (float)borderRadius / windowWidth, r_y = (float)borderRadius / windowHeight;

        poly[0]->vertices[0].position = {layout->x + r_x, layout->y};
        poly[0]->vertices[1].position = {layout->x + layout->w - r_x, layout->y};
        poly[0]->vertices[2].position = {layout->x + layout->w - r_x, layout->y + layout->h};
        poly[0]->vertices[3].position = {layout->x + r_x, layout->y + layout->h};

        poly[0]->vertices[4].position = {layout->x + layout->w - r_x, layout->y + r_y};
        poly[0]->vertices[5].position = {layout->x + layout->w, layout->y + r_y};
        poly[0]->vertices[6].position = {layout->x + layout->w, layout->y + layout->h - r_y};
        poly[0]->vertices[7].position = {layout->x + layout->w - r_x, layout->y + layout->h - r_y};

        poly[0]->vertices[8].position = {layout->x, layout->y + r_y};
        poly[0]->vertices[9].position = {layout->x + r_x, layout->y + r_y};
        poly[0]->vertices[10].position = {layout->x + r_x, layout->y + layout->h - r_y};
        poly[0]->vertices[11].position = {layout->x, layout->y + layout->h - r_y};

        poly[0]->vertices[12].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(22.5)), (layout->y + r_y) - r_y * cos(glm::radians(22.5))};
        poly[0]->vertices[13].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(45.0)), (layout->y + r_y) - r_y * cos(glm::radians(45.0))};
        poly[0]->vertices[14].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(67.5)), (layout->y + r_y) - r_y * cos(glm::radians(67.5))};

        poly[0]->vertices[15].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(67.5)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(67.5))};
        poly[0]->vertices[16].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(45.0)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(45.0))};
        poly[0]->vertices[17].position = {(layout->x + layout->w - r_x) + r_x * sin(glm::radians(22.5)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(22.5))};

        poly[0]->vertices[18].position = {(layout->x + r_x) - r_x * sin(glm::radians(22.5)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(22.5))};
        poly[0]->vertices[19].position = {(layout->x + r_x) - r_x * sin(glm::radians(45.0)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(45.0))};
        poly[0]->vertices[20].position = {(layout->x + r_x) - r_x * sin(glm::radians(67.5)), (layout->y + layout->h - r_y) + r_y * cos(glm::radians(67.5))};

        poly[0]->vertices[21].position = {(layout->x + r_x) - r_x * sin(glm::radians(67.5)), (layout->y + r_y) - r_y * cos(glm::radians(67.5))};
        poly[0]->vertices[22].position = {(layout->x + r_x) - r_x * sin(glm::radians(45.0)), (layout->y + r_y) - r_y * cos(glm::radians(45.0))};
        poly[0]->vertices[23].position = {(layout->x + r_x) - r_x * sin(glm::radians(22.5)), (layout->y + r_y) - r_y * cos(glm::radians(22.5))};

        poly[0]->applyVertexChange();
    }
}