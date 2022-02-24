#include "../../header/gl/object.hpp"

PointLight::PointLight(glm::vec3 color, glm::vec3 position)
{
    this->type = "point";
    this->color = color;
    this->position = position;
}
DirectLight::DirectLight(glm::vec3 color, glm::vec3 direction)
{
    this->type = "direct";
    this->color = color;
    this->direction = direction;
}
SpotLight::SpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction)
{
    this->type = "spot";
    this->color = color;
    this->position = position;
    this->direction = direction;
}

void Object::drawObjects(std::vector<Object *> &objects, Camera *camera, std::vector<Light *> &lights)
{
    for (Object *o : objects)
        if (o->light.type != "NULL")
            o->light.position = o->position;

    for (Object *o : objects)
        o->draw(camera, lights);
}

Object::Object(std::string filePath, Texture *texDiffuse, Texture *texSpecular, Shader *shader)
{
    this->texDiffuse = texDiffuse;
    this->texSpecular = texSpecular;

    this->shader = shader;
    use_texture = true;

    if (!load(filePath))
    {
        std::cout << "\033[1;31mError: Failed to load object:\033[0m " << filePath << '\n';
        return;
    }
}
Object::Object(std::string filePath, glm::vec3 meshColor, Shader *shader)
{
    color = meshColor;
    color_init = true;
    this->shader = shader;

    if (!load(filePath))
    {
        std::cout << "\033[1;31mError: Failed to load object:\033[0m " << filePath << '\n';
        return;
    }
}

Object::Object(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Texture *texDiffuse, Texture *texSpecular, Shader *shader)
{

    this->texDiffuse = texDiffuse;
    this->texSpecular = texSpecular;
    use_texture = true;

    this->shader = shader;

    createMesh(vertices, indices);
}
Object::Object(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, Shader *shader)
{
    this->shader = shader;

    createMesh(vertices, indices);
}

Object::~Object()
{
    delete mesh;
    if (texDelete)
    {
        if (texDiffuse)
            delete texDiffuse;
        if (texSpecular)
            delete texSpecular;
    }
}

bool Object::load(std::string filePath)
{
    std::string filePath_str = filePath;
    if (filePath_str.substr(filePath_str.find_last_of('.')) != ".obj")
        return false;

    std::ifstream f(filePath);
    if (!f.is_open())
        return false;

    std::vector<glm::vec3> v, vn;
    std::vector<glm::vec2> vt;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 errorColor{1, 0, 1};

    while (!f.eof())
    {
        char line[256], junk, junk2;
        f.getline(line, 256);
        std::strstream s;
        s << line;

        if (line[0] == 'v')
        {
            if (line[1] == 't')
            {
                glm::vec2 vec;
                s >> junk >> junk2 >> vec.x >> vec.y;
                vt.push_back(vec);
            }
            else
            {
                glm::vec3 vec;
                if (line[1] == 'n')
                {
                    s >> junk >> junk2 >> vec.x >> vec.y >> vec.z;
                    vn.push_back(vec);
                }
                else
                {
                    s >> junk >> vec.x >> vec.y >> vec.z;
                    v.push_back(vec);
                }
            }
        }
        else if (line[0] == 'f')
        {
            std::string f1, f2, f3;
            s >> junk >> f1 >> f2 >> f3;
            if (vt.size() > 0)
            {
                if (use_texture)
                {
                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f1.substr(0, f1.find('/'))) - 1], vn[std::stoi(f1.substr(f1.find_last_of('/') + 1)) - 1], vt[std::stoi(f1.substr(f1.find('/') + 1, f1.find_last_of('/'))) - 1]});

                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f2.substr(0, f2.find('/'))) - 1], vn[std::stoi(f2.substr(f2.find_last_of('/') + 1)) - 1], vt[std::stoi(f2.substr(f2.find('/') + 1, f2.find_last_of('/'))) - 1]});

                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f3.substr(0, f3.find('/'))) - 1], vn[std::stoi(f3.substr(f3.find_last_of('/') + 1)) - 1], vt[std::stoi(f3.substr(f3.find('/') + 1, f3.find_last_of('/'))) - 1]});
                }
                else
                {
                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f1.substr(0, f1.find('/'))) - 1], vn[std::stoi(f1.substr(f1.find_last_of('/') + 1)) - 1], color_init ? color : errorColor});

                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f2.substr(0, f2.find('/'))) - 1], vn[std::stoi(f2.substr(f2.find_last_of('/') + 1)) - 1], color_init ? color : errorColor});

                    indices.push_back(vertices.size());
                    vertices.push_back({v[std::stoi(f3.substr(0, f3.find('/'))) - 1], vn[std::stoi(f3.substr(f3.find_last_of('/') + 1)) - 1], color_init ? color : errorColor});
                }
            }
            else
            {
                use_texture = false;
                indices.push_back(vertices.size());
                vertices.push_back({v[std::stoi(f1.substr(0, f1.find("//"))) - 1], vn[std::stoi(f1.substr(f1.find("//") + 2)) - 1], color_init ? color : errorColor});

                indices.push_back(vertices.size());
                vertices.push_back({v[std::stoi(f2.substr(0, f2.find("//"))) - 1], vn[std::stoi(f2.substr(f2.find("//") + 2)) - 1], color_init ? color : errorColor});

                indices.push_back(vertices.size());
                vertices.push_back({v[std::stoi(f3.substr(0, f3.find("//"))) - 1], vn[std::stoi(f3.substr(f3.find("//") + 2)) - 1], color_init ? color : errorColor});
            }
        }
    }
    createMesh(vertices, indices);
    return true;
}

void Object::createMesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
    if (use_texture)
        mesh = new Mesh(vertices, indices, texDiffuse, texSpecular, shader);
    else
        mesh = new Mesh(vertices, indices, shader);
}

void Object::draw(Camera *camera, std::vector<Light *> &lights)
{
    double currentTime = glfwGetTime();
    if (currentKeyframe + 1 < keyframes.size() && keyframes[currentKeyframe + 1].time < currentTime)
        currentKeyframe++;
    if (currentKeyframe > -1)
    {
        if (currentKeyframe + 1 < keyframes.size())
        {
            keyframe &prevFrame = keyframes[currentKeyframe], &nextFrame = keyframes[currentKeyframe + 1];
            float timeDiff = nextFrame.time - prevFrame.time, currentTimeRelative = currentTime - prevFrame.time;
            position = glm::mix(prevFrame.position, nextFrame.position, currentTimeRelative / timeDiff);
            rotation = glm::mix(prevFrame.rotation, nextFrame.rotation, currentTimeRelative / timeDiff);
        }
        else
        {
            currentKeyframe = -1;
            position = keyframes.back().position;
            rotation = keyframes.back().rotation;
            keyframes.clear();
        }
    }
    if (rotateToCamera)
    {
        float rotZ = -atan(camera->orientation.x / camera->orientation.y);
        if (camera->orientation.y >= 0)
            rotZ -= M_PI;
        rotation = glm::angleAxis(rotZ, glm::vec3(0, 0, 1));
    }

    if (light.type != "NULL")
        light.position = position;

    shader->activate();

    glm::mat4 translateMatrix = glm::translate(glm::mat4(1), position),
              rotateMatrix = glm::mat4_cast(rotation),
              scaleMatrix = glm::scale(glm::mat4(1), scale),
              modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "modelMatrix"), 1, 0, glm::value_ptr(modelMatrix));

    unsigned int pointCount{0}, directCount{0}, spotCount{0};
    for (Light *l : lights)
    {
        if (l->type == "point")
        {
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("pointLightColor") + std::to_string(pointCount)).c_str()), 1, glm::value_ptr(l->color));
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("pointLightPos") + std::to_string(pointCount)).c_str()), 1, glm::value_ptr(l->position));
            pointCount++;
        }
        else if (l->type == "direct")
        {
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("directLightColor") + std::to_string(directCount)).c_str()), 1, glm::value_ptr(l->color));
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("directLightDir") + std::to_string(directCount)).c_str()), 1, glm::value_ptr(l->direction));
            directCount++;
        }
        else if (l->type == "spot")
        {
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("spotLightColor") + std::to_string(spotCount)).c_str()), 1, glm::value_ptr(l->color));
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("spotLightPos") + std::to_string(spotCount)).c_str()), 1, glm::value_ptr(l->position));
            glUniform3fv(glGetUniformLocation(shader->getID(), (std::string("spotLightDir") + std::to_string(spotCount)).c_str()), 1, glm::value_ptr(l->direction));
            spotCount++;
        }
    }

    mesh->draw(camera);
}
void Object::setDiffuseTexture(Texture *diffuse)
{
    texDiffuse = diffuse;
    use_texture = true;
    mesh->diffuse = diffuse;
}
void Object::setShader(Shader *shader)
{
    this->shader = shader;
}

void Object::saveTransformsAsDefaults()
{
    defaultPosition = position;
    defaultRotation = rotation;
    defaultScale = scale;
}
void Object::resetToDefaultTransforms(bool resetScale)
{
    position = defaultPosition;
    rotation = defaultRotation;
    if (resetScale)
        scale = defaultScale;

    keyframes.clear();
    currentKeyframe = -1;
}

void Object::emitLight(glm::vec3 color)
{
    light.type = "point";
    light.color = color;
}
void Object::emitLight(glm::vec3 color, glm::vec3 direction)
{
    light.type = "spot";
    light.color = color;
    light.direction = direction;
}

void Object::animate(std::vector<keyframe> keyframes, bool addStartPosition)
{
    if (addStartPosition)
        keyframes.insert(keyframes.begin(), {glfwGetTime(), position, rotation});
    this->keyframes = keyframes;
}

Shader *Object::skyboxShader = nullptr;

Object *Object::createSkybox(Texture *texDiffuse)
{
    if (!skyboxShader)
    {

        std::string skybox_vert = "#version 330 core\nlayout (location = 0) in vec3 aPos;\nlayout (location = 3) in vec2 aTex;\nout vec2 texCoord;\nuniform mat4 skyboxCamMatrix;\nvoid main()\n{\ntexCoord = aTex;\nvec4 pos = skyboxCamMatrix * vec4(aPos, 1);\ngl_Position = vec4(pos.x, pos.y, pos.w - .000001, pos.w);\n}",
                    skybox_frag = "#version 330 core\nout vec4 FragColor;\nin vec2 texCoord;\nuniform sampler2D texDiffuse;\nvoid main()\n{\nFragColor = texture(texDiffuse, texCoord);\n}";
        skyboxShader = new Shader(skybox_vert, skybox_frag, true);
    }

    std::vector<Vertex> vertices{
        {{1, 1, 1}, {0.748625, 0.666167}},
        {{1, -1, -1}, {0.499375, 0.333833}},
        {{1, 1, -1}, {0.748625, 0.333834}},
        {{1, -1, 1}, {0.499625, 0.666167}},
        {{-1, -1, -1}, {0.250375, 0.333833}},
        {{1, -1, -1}, {0.499625, 0.333833}},
        {{-1, -1, 1}, {0.250625, 0.666167}},
        {{-1, 1, -1}, {0.001375, 0.333833}},
        {{-1, -1, -1}, {0.250625, 0.333833}},
        {{-1, 1, 1}, {0.998625, 0.666167}},
        {{1, 1, -1}, {0.749375, 0.333834}},
        {{-1, 1, -1}, {0.998625, 0.333834}},
        {{-1, -1, -1}, {0.250375, 0.333833}},
        {{1, 1, -1}, {0.499625, 0.0015}},
        {{1, -1, -1}, {0.499625, 0.333833}},
        {{1, -1, 1}, {0.499625, 0.666167}},
        {{-1, 1, 1}, {0.250375, 0.9985}},
        {{-1, -1, 1}, {0.250375, 0.666167}},
        {{1, 1, 1}, {0.748625, 0.666167}},
        {{1, -1, 1}, {0.499375, 0.666167}},
        {{1, -1, -1}, {0.499375, 0.333833}},
        {{1, -1, 1}, {0.499625, 0.666167}},
        {{-1, -1, 1}, {0.250375, 0.666167}},
        {{-1, -1, -1}, {0.250375, 0.333833}},
        {{-1, -1, 1}, {0.250625, 0.666167}},
        {{-1, 1, 1}, {0.001375, 0.666166}},
        {{-1, 1, -1}, {0.001375, 0.333833}},
        {{-1, 1, 1}, {0.998625, 0.666167}},
        {{1, 1, 1}, {0.749375, 0.666167}},
        {{1, 1, -1}, {0.749375, 0.333834}},
        {{-1, -1, -1}, {0.250375, 0.333833}},
        {{-1, 1, -1}, {0.250375, 0.0015}},
        {{1, 1, -1}, {0.499625, 0.0015}},
        {{1, -1, 1}, {0.499625, 0.666167}},
        {{1, 1, 1}, {0.499625, 0.9985}},
        {{-1, 1, 1}, {0.250375, 0.9985}}};
    std::vector<unsigned int> indices{0, 2, 1, 3, 5, 4, 6, 8, 7, 9, 11, 10, 12, 14, 13, 15, 17, 16, 18, 20, 19, 21, 23, 22, 24, 26, 25, 27, 29, 28, 30, 32, 31, 33, 35, 34};

    Camera::createSkyboxCamMatrix = true;
    return new Object(vertices, indices, texDiffuse, nullptr, skyboxShader);
}
void Object::deleteSkyboxShader()
{
    if (skyboxShader)
    {
        delete skyboxShader;
        skyboxShader = nullptr;
    }
}