#include "../../header/gl/polygon.hpp"

Shader *Polygon::defaultShader = nullptr;
Shader *Polygon::textureShader = nullptr;

void Polygon::createShaders()
{
    Polygon::defaultShader = new Shader("#version 330 core\nlayout (location = 0) in vec2 aPos;\nlayout (location = 1) in vec4 aColor;\nout vec4 color;\nvoid main()\n{\ncolor = aColor;\ngl_Position = vec4(aPos.x, aPos.y, 0, 1);\n}", "#version 330 core\nout vec4 FragColor;\nin vec4 color;\nvoid main()\n{\nFragColor = color;\n}", true);
    Polygon::textureShader = new Shader("#version 330 core\nlayout (location = 0) in vec2 aPos;\nlayout (location = 2) in vec2 aTex;\nout vec2 texCoord;\nvoid main()\n{\ntexCoord = aTex;\ngl_Position = vec4(aPos.x, aPos.y, 0, 1);\n}", "#version 330 core\nout vec4 FragColor;\nin vec2 texCoord;\nuniform sampler2D texDiffuse;\nvoid main()\n{\nif(texture(texDiffuse, texCoord).a < .1)\ndiscard;\nFragColor = texture(texDiffuse, texCoord);\n}", true);
}

Vertex2D::Vertex2D(glm::vec2 position, glm::vec4 color)
{
    this->position = position;
    this->color = color;
}
Vertex2D::Vertex2D(glm::vec2 position, glm::vec3 color)
{
    this->position = position;
    this->color = glm::vec4(color, 1);
}
Vertex2D::Vertex2D(glm::vec2 position, glm::vec2 texture)
{
    this->position = position;
    this->texture = texture;
}
Vertex2D::Vertex2D(glm::vec2 position)
{
    this->position = position;
}

Polygon::Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, std::string image)
{
    if (!Polygon::defaultShader)
        Polygon::createShaders();

    this->vertices = vertices;
    this->indices = indices;
    if (image != "NULL")
    {
        diffuse = new Texture(image, Texture::DIFFUSE);
        tex_delete = true;
    }

    applyVertexChange();
}
Polygon::Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, Texture *image)
{
    if (!Polygon::defaultShader)
        Polygon::createShaders();

    this->vertices = vertices;
    this->indices = indices;
    this->diffuse = image;

    applyVertexChange();
}
Polygon::Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, unsigned int texID)
{
    if (!Polygon::defaultShader)
        Polygon::createShaders();

    this->vertices = vertices;
    this->indices = indices;
    this->texID = texID;

    applyVertexChange();
}

Polygon::~Polygon()
{
    delete vao;
    delete vbo;
    delete ebo;

    if (tex_delete)
        delete diffuse;
    else if (texID > 0)
        glDeleteTextures(1, &texID);
}

void Polygon::draw()
{
    if (customShader)
        customShader->activate();
    else if (diffuse || texID > 0)
        Polygon::textureShader->activate();
    else
        Polygon::defaultShader->activate();

    vao->bind();

    if (diffuse)
    {
        diffuse->texUnit(customShader ? customShader : Polygon::textureShader, "texDiffuse", 0);
        diffuse->bind();
    }
    else if (texID > 0)
    {
        glUniform1i(glGetUniformLocation(customShader ? customShader->getID() : Polygon::textureShader->getID(), "texDiffuse"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao->unbind();
}
void Polygon::applyVertexChange()
{
    std::vector<Vertex2D> vertOut = vertices;
    for (Vertex2D &v : vertOut)
    {
        v.position = {v.position.x * 2 - 1, v.position.y * -2 + 1};
        v.texture.y *= -1;
    }

    if (vao)
    {
        vao->bind();
        delete vbo;
        vbo = new VBO(vertOut);
    }
    else
    {
        vao = new VAO();
        vao->bind();
        vbo = new VBO(vertOut);
        ebo = new EBO(indices);
    }

    vao->linkAttrib(vbo, 0, 2, GL_FLOAT, sizeof(Vertex2D), (void *)0);
    if (diffuse || texID > 0)
        vao->linkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex2D), (void *)(6 * sizeof(float)));
    else
        vao->linkAttrib(vbo, 1, 4, GL_FLOAT, sizeof(Vertex2D), (void *)(2 * sizeof(float)));

    vao->unbind();
    vbo->unbind();
}
void Polygon::useCustomShader(Shader *shader)
{
    this->customShader = shader;
}

// Private Polygon structs

Polygon::VBO::VBO(std::vector<Vertex2D> &vertices)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);
}
Polygon::VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}
void Polygon::VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}
void Polygon::VBO::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Polygon::VAO::VAO()
{
    glGenVertexArrays(1, &id);
}
Polygon::VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}
void Polygon::VAO::linkAttrib(VBO *vbo, unsigned int layout, unsigned int numComponents, GLenum type, GLsizeiptr stride, void *offset)
{
    vbo->bind();
    glVertexAttribPointer(layout, numComponents, type, 0, stride, offset);
    glEnableVertexAttribArray(layout);
}
void Polygon::VAO::bind()
{
    glBindVertexArray(id);
}
void Polygon::VAO::unbind()
{
    glBindVertexArray(0);
}

Polygon::EBO::EBO(std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}
Polygon::EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}
void Polygon::EBO::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}