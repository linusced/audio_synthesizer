#include "../../header/gl/camera.hpp"

bool Camera::createSkyboxCamMatrix = false;

Camera::Camera(glm::vec3 position, glm::vec3 orientation, glm::vec3 up)
{
    this->position = position;
    this->orientation = glm::normalize(orientation);
    this->up = glm::normalize(up);
}

void Camera::updateProjMatrix(int width, int height)
{
    projMatrix = glm::perspective(glm::radians(fov_deg), (double)width / (double)height, nearPlane, farPlane);
}
void Camera::updateViewMatrix()
{
    glm::mat4 view{1};
    view = glm::lookAt(position, position + orientation, up);

    camMatrix = projMatrix * view;

    if (Camera::createSkyboxCamMatrix)
    {
        glm::mat4 skyView{1};
        skyView = glm::mat4(glm::mat3(glm::lookAt(position, position + orientation, up)));
        skyboxCamMatrix = projMatrix * skyView;
    }
}
void Camera::matrix(Shader *shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "camMatrix"), 1, 0, glm::value_ptr(camMatrix));
    if (Camera::createSkyboxCamMatrix)
        glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "skyboxCamMatrix"), 1, 0, glm::value_ptr(skyboxCamMatrix));
}
void Camera::resetPrevMousePos()
{
    prevMouseX = prevMouseY = -1;
}

void Camera::allInputs(GLFWwindow *window)
{
    mouseInputs(window);
    keyInputs(window);
}

void Camera::mouseInputs(GLFWwindow *window)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (prevMouseX != -1 && prevMouseY != -1)
    {
        glm::vec3 newOrientation = glm::rotate(orientation, (float)glm::radians((prevMouseY - mouseY) * sensitivity), glm::normalize(glm::cross(orientation, up)));
        if (abs(glm::angle(newOrientation, up) > (float)glm::radians(5.0)) && abs(glm::angle(newOrientation, -up) > (float)glm::radians(5.0)))
            orientation = newOrientation;

        orientation = glm::rotate(orientation, (float)glm::radians((prevMouseX - mouseX) * sensitivity), up);
    }

    prevMouseX = mouseX;
    prevMouseY = mouseY;
}

void Camera::keyInputs(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_UP))
    {
        glm::vec3 newOrientation = glm::rotate(orientation, (float)glm::radians(3.0), glm::normalize(glm::cross(orientation, up)));
        if (abs(glm::angle(newOrientation, up)) > (float)glm::radians(5.0))
            orientation = newOrientation;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN))
    {
        glm::vec3 newOrientation = glm::rotate(orientation, -(float)glm::radians(3.0), glm::normalize(glm::cross(orientation, up)));
        if (abs(glm::angle(newOrientation, -up)) > (float)glm::radians(5.0))
            orientation = newOrientation;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT))
        orientation = glm::rotate(orientation, (float)glm::radians(3.0), up);
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
        orientation = glm::rotate(orientation, -(float)glm::radians(3.0), up);

    if (glfwGetKey(window, GLFW_KEY_W))
        position += (float)speed * orientation;
    if (glfwGetKey(window, GLFW_KEY_A))
        position += (float)speed * -glm::normalize(glm::cross(orientation, up));
    if (glfwGetKey(window, GLFW_KEY_S))
        position += (float)speed * -orientation;
    if (glfwGetKey(window, GLFW_KEY_D))
        position += (float)speed * glm::normalize(glm::cross(orientation, up));

    if (glfwGetKey(window, GLFW_KEY_Q))
        position += (float)speed * up;
    if (glfwGetKey(window, GLFW_KEY_E))
        position += (float)speed * -up;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        speed = .4;
    else
        speed = .1;
}