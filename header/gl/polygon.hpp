#ifndef POLYGON
#define POLYGON

#include "material.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

struct Vertex2D
{
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 texture;
    Vertex2D(glm::vec2 position, glm::vec4 color);
    Vertex2D(glm::vec2 position, glm::vec3 color);
    Vertex2D(glm::vec2 position, glm::vec2 texture);
    Vertex2D(glm::vec2 position);
};

class Polygon
{
private:
    class VBO
    {
    private:
        unsigned int id;

    public:
        VBO(std::vector<Vertex2D> &vertices);
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
    {
    private:
        unsigned int id;

    public:
        EBO(std::vector<unsigned int> &indices);
        ~EBO();

        void unbind();
    };

    static Shader *defaultShader, *textureShader;
    static void createShaders();

    VBO *vbo = nullptr;
    VAO *vao = nullptr;
    EBO *ebo = nullptr;

    Texture *diffuse = nullptr;
    unsigned int texID = 0;
    bool tex_delete = false;
    Shader *customShader = nullptr;

public:
    std::vector<Vertex2D> vertices;
    std::vector<unsigned int> indices;

    Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, std::string image = "NULL");
    Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, Texture *image);
    Polygon(std::vector<Vertex2D> vertices, std::vector<unsigned int> indices, unsigned int texID);
    ~Polygon();

    void draw();
    void applyVertexChange();
    void useCustomShader(Shader *shader);
};

#endif