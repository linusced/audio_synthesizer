#ifndef MATERIAL
#define MATERIAL

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif

class Shader
{
private:
    unsigned int id;

    bool loadFile(std::string filename, std::string &outputData);
    void compileErrors(unsigned int shader, std::string type);

public:
    Shader(std::string vertexFile, std::string fragmentFile, bool isRawCode = false);
    ~Shader();

    void activate();
    unsigned int getID();
};

class Texture
{
public:
    enum TEX_TYPE
    {
        DIFFUSE,
        SPECULAR
    };

private:
    unsigned int id, slot;
    int widthImg, heightImg, numColCh;
    TEX_TYPE type;

public:
    Texture(std::string filePath, TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);
    Texture(unsigned char *bytes, int widthImg, int heightImg, int numColCh, TEX_TYPE texType, int wrapX = GL_REPEAT, int wrapY = GL_REPEAT, int scaleAlgorithm = GL_NEAREST);
    ~Texture();

    void texUnit(Shader *shader, std::string uniform, unsigned int unit);
    void bind();

    unsigned int getID();
    TEX_TYPE getType();

    int getWidth();
    int getHeight();
    int getColorChannels();
};

#endif