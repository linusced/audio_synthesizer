#ifndef MESH
#define MESH

#include <vector>
#include "camera.hpp"

struct Vertex
{
    glm::vec3 position,
        normal,
        color;
    glm::vec2 texture;

    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texture);
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec3 color);
    Vertex(glm::vec3 position, glm::vec3 normal);
    Vertex(glm::vec3 position, glm::vec2 texture);
};

class Mesh
{
private:
    class VBO
    {private:
        unsigned int id;

    public:

        VBO(std::vector<Vertex> &vertices);
        ~VBO();

        void bind();
        void unbind();
    };
    class VAO
    {
    private:
        unsigned int id;

    public:
        VAO();
        ~VAO();

        void linkAttrib(VBO *vbo, unsigned int layout, unsigned int numComponents, GLenum type, GLsizeiptr stride, void *offset);
        void bind();
        void unbind();
    };
    class EBO
    {private:
        unsigned int id;

    public:

        EBO(std::vector<unsigned int> &indices);
        ~EBO();

        void unbind();
    };

    VAO *vao = nullptr;
    VBO *vbo = nullptr;
    EBO *ebo = nullptr;

public:
    Shader *shader = nullptr;
    Texture *diffuse = nullptr;
    Texture *specular = nullptr;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Texture *texDiffuse, Texture *texSpecular, Shader *shader);
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader);
    ~Mesh();

    void draw(Camera *camera);
    void applyVertexChange();
};

#endif