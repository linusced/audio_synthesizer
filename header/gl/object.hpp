#ifndef OBJECT
#define OBJECT

#include "mesh.hpp"
#include <glm/gtx/quaternion.hpp>
#include <strstream>
#include <map>

#ifndef STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#endif

struct Light
{
    std::string type{"NULL"};
    glm::vec3 color, position, direction;
};
struct PointLight : public Light
{
    PointLight(glm::vec3 color, glm::vec3 position);
};
struct DirectLight : public Light
{
    DirectLight(glm::vec3 color, glm::vec3 direction);
};
struct SpotLight : public Light
{
    SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction);
};

class Object
{
private:
    Texture *texDiffuse = nullptr;
    Texture *texSpecular = nullptr;
    glm::vec3 color;
    Shader *shader = nullptr;
    glm::vec3 defaultPosition, defaultScale;
    glm::quat defaultRotation;
    bool use_texture = false, color_init = false;

    void createMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices);
    bool load(std::string filePath);

    static Shader *skyboxShader;

public:
    struct keyframe
    {
        double time;
        glm::vec3 position;
        glm::quat rotation;
    };
    static void drawObjects(std::vector<Object *> &objects, Camera *camera, std::vector<Light *> &lights);

    Mesh *mesh;
    glm::vec3 scale{1, 1, 1}, position{0, 0, 0};
    glm::quat rotation{1, 0, 0, 0};
    Light light;
    bool rotateToCamera = false, texDelete = false;

    Object(std::string filePath, Texture *texDiffuse, Texture *texSpecular, Shader *shader);
    Object(std::string filePath, glm::vec3 meshColor, Shader *shader);

    Object(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Texture *texDiffuse, Texture *texSpecular, Shader *shader);
    Object(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader);

    ~Object();

    void draw(Camera *camera, std::vector<Light *> &lights);
    void setDiffuseTexture(Texture *diffuse);
    void setShader(Shader *sh);
    void saveTransformsAsDefaults();
    void resetToDefaultTransforms(bool resetScale = false);

    // Point light
    void emitLight(glm::vec3 color);
    // Spot light
    void emitLight(glm::vec3 color, glm::vec3 direction);

    void animate(std::vector<keyframe> keyframes, bool addStartPosition = true);

    static Object *createSkybox(Texture *texDiffuse);
    static void deleteSkyboxShader();

private:
    int currentKeyframe = -1;
    std::vector<keyframe> keyframes;
};

#endif