#include "../../header/gl/scene.hpp"

ObjectGroup::~ObjectGroup()
{
    for (auto i : textures)
        delete i;
    for (auto i : objects)
        delete i;
}

std::vector<Object *> &ObjectGroup::getObjects()
{
    return objects;
}

Texture *ObjectGroup::getTexture(size_t index)
{
    if (index < textures.size())
        return textures[index];
    return nullptr;
}
Texture *ObjectGroup::getTexture(std::string name)
{
    auto i = textures_named.find(name);
    if (i != textures_named.end())
        return i->second;
    return nullptr;
}

Object *ObjectGroup::getObject(size_t index)
{
    if (index < objects.size())
        return objects[index];
    return nullptr;
}
Object *ObjectGroup::getObject(std::string name)
{
    auto i = objects_named.find(name);
    if (i != objects_named.end())
        return i->second;
    return nullptr;
}

void ObjectGroup::createTexture(std::string name, std::string filePath, Texture::TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    if (!__load)
    {
        __textures_1.push_back({name, filePath, texType, wrapX, wrapY, scaleAlgorithm});
        return;
    }
    Texture *t = new Texture(filePath, texType, wrapX, wrapY, scaleAlgorithm);
    textures.push_back(t);
    if (name != "NULL")
        textures_named[name] = t;
}
void ObjectGroup::createTexture(std::string name, unsigned char *bytes, int widthImg, int heightImg, int numColCh, Texture::TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    if (!__load)
    {
        __textures_2.push_back({name, bytes, widthImg, heightImg, numColCh, texType, wrapX, wrapY, scaleAlgorithm});
        return;
    }
    Texture *t = new Texture(bytes, widthImg, heightImg, numColCh, texType, wrapX, wrapY, scaleAlgorithm);
    textures.push_back(t);
    if (name != "NULL")
        textures_named[name] = t;
}

void ObjectGroup::createObject(std::string filePath, std::string texDiffuseName, std::string texSpecularName, Shader *shader, std::string name)
{
    if (!__load)
    {
        __objects_1.push_back({filePath, texDiffuseName, texSpecularName, shader, name});
        return;
    }
    Object *o = new Object(filePath, textures_named[texDiffuseName], textures_named[texSpecularName], shader);
    objects.push_back(o);
    if (name != "NULL")
        objects_named[name] = o;
}
void ObjectGroup::createObject(std::string filePath, glm::vec3 meshColor, Shader *shader, std::string name)
{
    if (!__load)
    {
        __objects_2.push_back({filePath, meshColor, shader, name});
        return;
    }
    Object *o = new Object(filePath, meshColor, shader);
    objects.push_back(o);
    if (name != "NULL")
        objects_named[name] = o;
}
void ObjectGroup::createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texDiffuseName, std::string texSpecularName, Shader *shader, std::string name)
{
    if (!__load)
    {
        __objects_3.push_back({vertices, indices, texDiffuseName, texSpecularName, shader, name});
        return;
    }
    Object *o = new Object(vertices, indices, textures_named[texDiffuseName], textures_named[texSpecularName], shader);
    objects.push_back(o);
    if (name != "NULL")
        objects_named[name] = o;
}
void ObjectGroup::createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader, std::string name)
{
    if (!__load)
    {
        __objects_4.push_back({vertices, indices, shader, name});
        return;
    }
    Object *o = new Object(vertices, indices, shader);
    objects.push_back(o);
    if (name != "NULL")
        objects_named[name] = o;
}

void ObjectGroup::createSkybox(std::string texDiffuseName)
{
    if (!__load)
    {
        __skybox.push_back(texDiffuseName);
        return;
    }
    objects.push_back(Object::createSkybox(textures_named[texDiffuseName]));
}

size_t ObjectGroup::getLoadSize()
{
    return __textures_1.size() + __textures_2.size() + __objects_1.size() + __objects_2.size() + __objects_3.size() + __objects_4.size() + __skybox.size();
}
template <typename callback>
void ObjectGroup::load(callback progress)
{
    __load = true;

    for (auto &i : __textures_1)
    {
        createTexture(i.name, i.filePath, i.texType, i.wrapX, i.wrapY, i.scaleAlgorithm);
        progress();
    }
    __textures_1.clear();
    for (auto &i : __textures_2)
    {
        createTexture(i.name, i.bytes, i.widthImg, i.heightImg, i.numColCh, i.texType, i.wrapX, i.wrapY, i.scaleAlgorithm);
        progress();
    }
    __textures_2.clear();

    for (auto &i : __objects_1)
    {
        createObject(i.filePath, i.texDiffuseName, i.texSpecularName, i.shader, i.name);
        progress();
    }
    __objects_1.clear();
    for (auto &i : __objects_2)
    {
        createObject(i.filePath, i.meshColor, i.shader, i.name);
        progress();
    }
    __objects_2.clear();
    for (auto &i : __objects_3)
    {
        createObject(i.vertices, i.indices, i.texDiffuseName, i.texSpecularName, i.shader, i.name);
        progress();
    }
    __objects_3.clear();
    for (auto &i : __objects_4)
    {
        createObject(i.vertices, i.indices, i.shader, i.name);
        progress();
    }
    __objects_4.clear();

    for (auto &i : __skybox)
    {
        createSkybox(i);
        progress();
    }
    __skybox.clear();
}

Scene::Scene(GLFWwindow *window, int *windowWidth, int *windowHeight)
{
    this->window = window;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;
}
Scene::~Scene()
{
    for (auto i : cameras)
        delete i;
    for (auto i : lights)
        delete i;
    for (auto i : shaders)
        delete i;
    for (auto i : groups)
        delete i.second;
}

Camera *Scene::getCamera(size_t index)
{
    if (index < cameras.size())
        return cameras[index];
    return nullptr;
}
Camera *Scene::getCamera(std::string name)
{
    auto i = cameras_named.find(name);
    if (i != cameras_named.end())
        return i->second;
    return nullptr;
}

Light *Scene::getLight(size_t index)
{
    if (index < lights.size())
        return lights[index];
    return nullptr;
}
Light *Scene::getLight(std::string name)
{
    auto i = lights_named.find(name);
    if (i != lights_named.end())
        return i->second;
    return nullptr;
}

Shader *Scene::getShader(size_t index)
{
    if (index < shaders.size())
        return shaders[index];
    return nullptr;
}
Shader *Scene::getShader(std::string name)
{
    auto i = shaders_named.find(name);
    if (i != shaders_named.end())
        return i->second;
    return nullptr;
}

Texture *Scene::getTexture(size_t index)
{
    return mainGroup.getTexture(index);
}
Texture *Scene::getTexture(std::string name)
{
    return mainGroup.getTexture(name);
}

Object *Scene::getObject(size_t index)
{
    return mainGroup.getObject(index);
}
Object *Scene::getObject(std::string name)
{
    return mainGroup.getObject(name);
}

ObjectGroup *Scene::getObjectGroup(std::string name)
{
    auto i = groups.find(name);
    if (i != groups.end())
        return i->second;
    return nullptr;
}

void Scene::setActiveCamera(size_t index)
{
    if (index < cameras.size())
        activeCam = cameras[index];
}
void Scene::setActiveCamera(std::string name)
{
    auto i = cameras_named.find(name);
    if (i != cameras_named.end())
        activeCam = i->second;
}

void Scene::createCamera(std::string name, glm::vec3 position, glm::vec3 orientation, glm::vec3 up)
{
    Camera *c = new Camera(position, orientation, up);
    cameras.push_back(c);
    if (name != "NULL")
        cameras_named[name] = c;
    if (!activeCam)
        activeCam = c;
}

void Scene::createPointLight(glm::vec3 color, glm::vec3 position, std::string name)
{
    Light *l = new PointLight(color, position);
    lights.push_back(l);
    if (name != "NULL")
        lights_named[name] = l;
}
void Scene::createDirectLight(glm::vec3 color, glm::vec3 direction, std::string name)
{
    Light *l = new DirectLight(color, direction);
    lights.push_back(l);
    if (name != "NULL")
        lights_named[name] = l;
}
void Scene::createSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, std::string name)
{
    Light *l = new SpotLight(color, position, direction);
    lights.push_back(l);
    if (name != "NULL")
        lights_named[name] = l;
}

void Scene::createShader(std::string name, std::string vertexFile, std::string fragmentFile, bool isRawCode)
{
    Shader *s = new Shader(vertexFile, fragmentFile, isRawCode);
    shaders.push_back(s);
    if (name != "NULL")
        shaders_named[name] = s;
}

void Scene::createTexture(std::string name, std::string filePath, Texture::TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    mainGroup.createTexture(name, filePath, texType, wrapX, wrapY, scaleAlgorithm);
}
void Scene::createTexture(std::string name, unsigned char *bytes, int widthImg, int heightImg, int numColCh, Texture::TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    mainGroup.createTexture(name, bytes, widthImg, heightImg, numColCh, texType, wrapX, wrapY, scaleAlgorithm);
}

void Scene::createObject(std::string filePath, std::string texDiffuseName, std::string texSpecularName, std::string shaderName, std::string name)
{
    mainGroup.createObject(filePath, texDiffuseName, texSpecularName, shaders_named[shaderName], name);
}
void Scene::createObject(std::string filePath, glm::vec3 meshColor, std::string shaderName, std::string name)
{
    mainGroup.createObject(filePath, meshColor, shaders_named[shaderName], name);
}
void Scene::createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string texDiffuseName, std::string texSpecularName, std::string shaderName, std::string name)
{
    mainGroup.createObject(vertices, indices, texDiffuseName, texSpecularName, shaders_named[shaderName], name);
}
void Scene::createObject(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::string shaderName, std::string name)
{
    mainGroup.createObject(vertices, indices, shaders_named[shaderName], name);
}

void Scene::createSkybox(std::string texDiffuseName)
{
    mainGroup.createSkybox(texDiffuseName);
}

std::thread *Scene::loadThread = nullptr;
void Scene::loadThreadFunc(Scene *self, GLFWwindow *window, float *progress, bool *updateProgress)
{
    size_t loadIndex = 0, loadSize = 0;
    auto __progress = [&]()
    {
        if (loadSize > 0)
        {
            loadIndex++;
            progress[0] = loadIndex / (float)loadSize;
        }

        updateProgress[0] = true;
        if (progress[0] < 1)
            while (updateProgress[0])
            {
            }
    };
    __progress();

    loadSize = self->mainGroup.getLoadSize();
    for (auto i : self->groups)
        loadSize += i.second->getLoadSize();

    glfwMakeContextCurrent(window);

    self->mainGroup.load(__progress);
    for (auto i : self->groups)
        i.second->load(__progress);

    progress[0] = 1;
}

gui::Master *Scene::createLoadGUI()
{
    gui::Master *m = new gui::Master();

    m->saveConstraints("progressBarContainer", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::RELATIVE_CONSTRAINT_INVERSE, 0, gui::RELATIVE_CONSTRAINT, 1, gui::PIXEL_CONSTRAINT, 44));
    m->saveNamedChild("progressBarContainer", new gui::native::Container({.2, .2, .2, 1}, 0, m->getConstraints("progressBarContainer")), true);

    m->saveConstraints("progressBar", new gui::Constraints(gui::RELATIVE_CONSTRAINT, 0, gui::PIXEL_CONSTRAINT_INVERSE, 2, gui::RELATIVE_CONSTRAINT, 0, gui::PIXEL_CONSTRAINT, 40));
    m->saveNamedChild("progressBar", new gui::native::Container(glm::vec4(1), 0, m->getConstraints("progressBar")));
    m->getNamedChild("progressBarContainer")->addChild(m->getNamedChild("progressBar"));
    return m;
}

void Scene::load(bool displayGUI)
{
    float progress = 0;
    bool updateProgress = false;

    gui::Master *loadGUI = nullptr;

    if (displayGUI)
        loadGUI = createLoadGUI();

    loadThread = new std::thread(Scene::loadThreadFunc, this, window, &progress, &updateProgress);

    while (progress < 1)
        if (updateProgress)
        {
            if (displayGUI)
            {
                glClear(GL_COLOR_BUFFER_BIT);

                if (loadGUI->getConstraints("progressBar"))
                    loadGUI->getConstraints("progressBar")->setW(progress);
                loadGUI->resize(*windowWidth, *windowHeight);
                loadGUI->draw();

                glfwSwapBuffers(window);
            }
            updateProgress = false;
            glfwPollEvents();
        }

    if (displayGUI)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (loadGUI->getConstraints("progressBar"))
            loadGUI->getConstraints("progressBar")->setW(progress);
        loadGUI->resize(*windowWidth, *windowHeight);
        loadGUI->draw();

        glfwSwapBuffers(window);
    }
    glfwPollEvents();

    loadThread->join();

    delete loadThread;
    loadThread = nullptr;
    delete loadGUI;
}

void Scene::resize()
{
    for (auto c : cameras)
        c->updateProjMatrix(*windowWidth, *windowHeight);
}
void Scene::inputs()
{
    if (activeCam)
        activeCam->allInputs(window);
}
void Scene::draw()
{
    if (activeCam)
    {
        activeCam->updateViewMatrix();

        if (mainGroup.draw)
            Object::drawObjects(mainGroup.getObjects(), activeCam, lights);

        for (auto g : groups)
            if (g.second->draw)
                Object::drawObjects(g.second->getObjects(), activeCam, lights);
    }
}