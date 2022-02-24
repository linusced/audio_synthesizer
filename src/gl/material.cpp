#include "../../header/gl/material.hpp"

bool Shader::loadFile(std::string path, std::string &outputData)
{
    std::ifstream in(path, std::ios::binary);
    if (in.is_open())
    {
        in.seekg(0, std::ios::end);
        outputData.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&outputData[0], outputData.size());
        in.close();
        return true;
    }
    return false;
}

Shader::Shader(std::string vertexFile, std::string fragmentFile, bool isRawCode)
{
    std::string vertexCode, fragmentCode;
    if (isRawCode)
    {
        vertexCode = vertexFile;
        fragmentCode = fragmentFile;
    }
    else
    {
        if (!loadFile(vertexFile, vertexCode))
        {
            std::cout << "\033[1;31mError: Failed to open vertex shader file:\033[0m " << vertexFile << '\n';
            return;
        }
        if (!loadFile(fragmentFile, fragmentCode))
        {
            std::cout << "\033[1;31mError: Failed to open fragment shader file:\033[0m " << fragmentFile << '\n';
            return;
        }
    }

    const char *vertexSource = vertexCode.c_str(),
               *fragmentSource = fragmentCode.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    compileErrors(fragmentShader, "FRAGMENT");

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);

    glLinkProgram(id);
    compileErrors(id, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::activate()
{
    glUseProgram(id);
}
unsigned int Shader::getID()
{
    return id;
}

void Shader::compileErrors(unsigned int shader, std::string type)
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (!hasCompiled)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "\033[1;31mSHADER_COMPILATION_ERROR for: " << type << "\033[0m\n"
                      << infoLog << '\n';
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (!hasCompiled)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "\033[1;31mSHADER_LINKING_ERROR for: " << type << "\033[0m\n"
                      << infoLog << '\n';
        }
    }
}

Texture::Texture(std::string filePath, TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    type = texType;
    slot = (unsigned int)texType;

    unsigned char *bytes;

    stbi_set_flip_vertically_on_load(true);
    bytes = stbi_load(filePath.c_str(), &widthImg, &heightImg, &numColCh, 0);
    if ((widthImg < 1 || heightImg < 1 || numColCh < 1) && filePath != "NULL")
        std::cout << "\033[1;31mError: Failed to load texture file:\033[0m " << filePath << '\n';

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);

    GLenum format = GL_RGBA;
    if (numColCh == 1)
        format = GL_RED;
    else if (numColCh == 2)
        format = GL_RG;
    else if (numColCh == 3)
        format = GL_RGB;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaleAlgorithm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaleAlgorithm);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(unsigned char *bytes, int widthImg, int heightImg, int numColCh, TEX_TYPE texType, int wrapX, int wrapY, int scaleAlgorithm)
{
    type = texType;
    slot = (unsigned int)texType;

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);

    GLenum format = GL_RGBA;
    if (numColCh == 1)
        format = GL_RED;
    else if (numColCh == 2)
        format = GL_RG;
    else if (numColCh == 3)
        format = GL_RGB;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapX);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapY);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaleAlgorithm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaleAlgorithm);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::texUnit(Shader *shader, std::string uniform, unsigned int unit)
{
    shader->activate();
    glUniform1i(glGetUniformLocation(shader->getID(), uniform.c_str()), unit);
}
void Texture::bind()
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

unsigned int Texture::getID()
{
    return id;
}
Texture::TEX_TYPE Texture::getType()
{
    return type;
}

int Texture::getWidth()
{
    return widthImg;
}
int Texture::getHeight()
{
    return heightImg;
}
int Texture::getColorChannels()
{
    return numColCh;
}