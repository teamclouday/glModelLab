// This code is mainly copied from the book with minor changes
// check https://learnopengl.com to learn more!
#pragma once
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <vector>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW         = -90.0f;
const GLfloat PITCH       =   0.0f;
const GLfloat SPEED       =   0.002f;
const GLfloat SENSITIVITY =   0.1f;
const GLfloat ZOOM        =  45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;

    GLfloat MovementSpeed;
    GLfloat MouseSenditivity;
    GLfloat Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           GLfloat yaw = YAW, GLfloat pitch = PITCH);
    // constructor with scaler values
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
           GLfloat upX, GLfloat upY, GLfloat upZ,
           GLfloat yaw = YAW, GLfloat pitch = PITCH);

    // return view matrix calculated using Eular Angles and LookAt Matrix
    glm::mat4 GetViewMatrix();
    // process input from keyboard
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    // process input from mouse
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    // process input from scroll
    void ProcessMouseScroll(GLfloat yoffset);
    // reset all values
    void reset();

private:
    void updateCameraVectors();
};