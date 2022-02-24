#include "../../header/gl/mesh.hpp"

Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture)
{
    this->position = position;
    this->normal = normal;
    this->texture = texture;
    this->color = {0, 0, 0};
}
Vertex::Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color)
{
    this->position = position;
    this->normal = normal;
    this->color = color;
}
Vertex::Vertex(glm::vec3 position, glm::vec3 normal)
{
    this->position = position;
    this->normal = normal;
    this->color = {0, 0, 0};
}
Vertex::Vertex(glm::vec3 position, glm::vec2 texture)
{
    this->position = position;
    this->texture = texture;
    this->color = {0, 0, 0};
}

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Texture *texDiffuse, Texture *texSpecular, Shader *shader)
{
    this->vertices = vertices;
    this->indices = indices;
    this->diffuse = texDiffuse;
    this->specular = texSpecular;
    this->shader = shader;

    vao = new VAO();
    vao->bind();
    vbo = new VBO(vertices);
    ebo = new EBO(indices);

    vao->linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    vao->linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
    vao->linkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)(9 * sizeof(float)));

    vao->unbind();
    ebo->unbind();
    vbo->unbind();
}
Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader)
{
    this->vertices = vertices;
    this->indices = indices;
    this->shader = shader;

    vao = new VAO();
    vao->bind();
    vbo = new VBO(vertices);
    ebo = new EBO(indices);

    vao->linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    vao->linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
    vao->linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));

    vao->unbind();
    ebo->unbind();
    vbo->unbind();
}
Mesh::~Mesh()
{
    delete vao;
    delete vbo;
    delete ebo;
}

void Mesh::draw(Camera *camera)
{
    shader->activate();
    vao->bind();

    if (diffuse)
    {
        diffuse->texUnit(shader, "texDiffuse", 0);
        diffuse->bind();
    }
    if (specular)
    {
        specular->texUnit(shader, "texSpecular", 1);
        specular->bind();
    }

    glUniform3f(glGetUniformLocation(shader->getID(), "camPos"), camera->position.x, camera->position.y, camera->position.z);
    camera->matrix(shader);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao->unbind();
}
void Mesh::applyVertexChange()
{
    vao->bind();

    delete vbo;
    vbo = new VBO(vertices);

    vao->linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);
    vao->linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)(3 * sizeof(float)));
    vao->linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)(6 * sizeof(float)));
    vao->linkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)(9 * sizeof(float)));

    vao->unbind();
    vbo->unbind();
}

// Private Mesh structs

Mesh::VBO::VBO(std::vector<Vertex> &vertices)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
Mesh::VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}
void Mesh::VBO::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}
void Mesh::VBO::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::VAO::VAO()
{
    glGenVertexArrays(1, &id);
}
Mesh::VAO::~VAO()
{
    glDeleteVertexArrays(1, &id);
}
void Mesh::VAO::linkAttrib(VBO *vbo, unsigned int layout, unsigned int numComponents, GLenum type, GLsizeiptr stride, void *offset)
{
    vbo->bind();
    glVertexAttribPointer(layout, numComponents, type, 0, stride, offset);
    glEnableVertexAttribArray(layout);
}
void Mesh::VAO::bind()
{
    glBindVertexArray(id);
}
void Mesh::VAO::unbind()
{
    glBindVertexArray(0);
}

Mesh::EBO::EBO(std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}
Mesh::EBO::~EBO()
{
    glDeleteBuffers(1, &id);
}
void Mesh::EBO::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}