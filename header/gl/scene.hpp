#ifndef SCENE
#define SCENE

#include "object.hpp"
#include "gui.hpp"
#include <thread>

class ObjectGroup
{
private:
    std::vector<Texture *> textures;
    std::vector<Object *> objects;

    std::map<std::string, Texture *> textures_named;
    std::map<std::string, Object *> objects_named;

    bool __load = false;

    struct __createTexture_1
    {
        std::string name, filePath;
        Texture::TEX_TYPE texType;
        int wrapX, wrapY, scaleAlgorithm;
    };
    std::vector<__createTexture_1> __textures_1;
    struct __createTexture_2
    {
        std::string name;
        unsigned char *bytes;
        int widthImg, heightImg, numColCh;
        Texture::TEX_TYPE texType;
        int wrapX, wrapY, scaleAlgorithm;
    };
    std::vector<__createTexture_2> __textures_2;

    struct __createObject_1
    {
        std::string filePath, texDiffuseName, texSpecularName;
        Shader *shader;
        std::string name;
    };
    std::vector<__createObject_1> __objects_1;
    struct __createObject_2
    {
        std::string filePath;
        glm::vec3 meshColor;
        Shader *shader;
        std::string name;
    };
    std::vector<__createObject_2> __objects_2;
    struct __createObject_3
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::string texDiffuseName, texSpecularName;
        Shader *shader;
        std::string name;
    };
    std::vector<__createObject_3> __objects_3;
    struct __createObject_4
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Shader *shader;
        std::string name;
    };
    std::vector<__createObject_4> __objects_4;

    std::vector<std::string> __skybox;

public:
    bool draw = true;

    ~ObjectGroup();

    std::vector<Object *> &getObjects();

    Texture *getTexture(size_t index);
    Texture *getTexture(std::string name);

    Object *getObject(size_t index);
    Object *getObject(std::string name);

    void createTexture(std::string name, std::string filePath, Texture::TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);
    void createTexture(std::string name, unsigned char *bytes, int widthImg, int heightImg, int numColCh, Texture::TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);

    void createObject(std::string filePath, std::string texDiffuseName, std::string texSpecularName, Shader *shader, std::string name = "NULL");
    void createObject(std::string filePath, glm::vec3 meshColor, Shader *shader, std::string name = "NULL");
    void createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texDiffuseName, std::string texSpecularName, Shader *shader, std::string name = "NULL");
    void createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader, std::string name = "NULL");

    void createSkybox(std::string texDiffuseName);

    size_t getLoadSize();
    template <typename callback>
    void load(callback progress);
};

class Scene
{
protected:
    GLFWwindow *window = nullptr;
    int *windowWidth = nullptr, *windowHeight = nullptr;

    std::map<std::string, Camera *> cameras_named;
    std::map<std::string, Light *> lights_named;
    std::map<std::string, Shader *> shaders_named;

    static std::thread *loadThread;
    static void loadThreadFunc(Scene *self, GLFWwindow *window, float *progress, bool *updateProgress);

    virtual gui::Master *createLoadGUI();

public:
    Camera *activeCam = nullptr;
    std::vector<Camera *> cameras;
    std::vector<Shader *> shaders;
    std::vector<Light *> lights;

    ObjectGroup mainGroup;
    std::map<std::string, ObjectGroup *> groups;

    Scene(GLFWwindow *window, int *windowWidth, int *windowHeight);
    ~Scene();

    Camera *getCamera(size_t index);
    Camera *getCamera(std::string name);

    Light *getLight(size_t index);
    Light *getLight(std::string name);

    Shader *getShader(size_t index);
    Shader *getShader(std::string name);

    Texture *getTexture(size_t index);
    Texture *getTexture(std::string name);

    Object *getObject(size_t index);
    Object *getObject(std::string name);

    ObjectGroup *getObjectGroup(std::string name);

    void setActiveCamera(size_t index);
    void setActiveCamera(std::string name);

    void createCamera(std::string name, glm::vec3 position, glm::vec3 orientation, glm::vec3 up);

    void createPointLight(glm::vec3 color, glm::vec3 position, std::string name = "NULL");
    void createDirectLight(glm::vec3 color, glm::vec3 direction, std::string name = "NULL");
    void createSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, std::string name = "NULL");

    void createShader(std::string name, std::string vertexFile, std::string fragmentFile, bool isRawCode = false);

    void createTexture(std::string name, std::string filePath, Texture::TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);
    void createTexture(std::string name, unsigned char *bytes, int widthImg, int heightImg, int numColCh, Texture::TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);

    void createObject(std::string filePath, std::string texDiffuseName, std::string texSpecularName, std::string shaderName, std::string name = "NULL");
    void createObject(std::string filePath, glm::vec3 meshColor, std::string shaderName, std::string name = "NULL");
    void createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texDiffuseName, std::string texSpecularName, std::string shaderName, std::string name = "NULL");
    void createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string shaderName, std::string name = "NULL");

    void createSkybox(std::string texDiffuseName);

    void load(bool displayGUI);

    virtual void resize();
    virtual void inputs();
    virtual void draw();
};

#endif