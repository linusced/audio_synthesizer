#ifndef CAMERA
#define CAMERA

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "material.hpp"

class Camera
{
protected:
    double prevMouseX{-1}, prevMouseY{-1};
    glm::mat4 projMatrix{1}, camMatrix{1}, skyboxCamMatrix{1};

public:
    static bool createSkyboxCamMatrix;

    glm::vec3 position, orientation, up;
    double speed = .1, sensitivity = .2, fov_deg = 90, nearPlane = .1, farPlane = 100;

    Camera(glm::vec3 position, glm::vec3 orientation, glm::vec3 up);

    virtual void updateProjMatrix(int width, int height);
    virtual void updateViewMatrix();
    void matrix(Shader *shader);
    void resetPrevMousePos();

    void allInputs(GLFWwindow *window);

    virtual void mouseInputs(GLFWwindow *window);
    virtual void keyInputs(GLFWwindow *window);
};

#endif