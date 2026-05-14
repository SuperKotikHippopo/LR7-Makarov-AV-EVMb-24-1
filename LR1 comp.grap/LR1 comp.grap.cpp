#define GLFW_DLL
#define GLEW_DLL
#include "glew-2.1.0/include/GL/glew.h"
#include "glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include "shader.h"
#include "Model.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool  firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 512.0f;
float lastY = 384.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos = glm::vec3(5.0f, 8.0f, 5.0f);

float angle1 = 0.0f;  
float angle2 = 0.0f;   
float pos3Y = 0.0f;   

const float ROT_SPEED = 60.0f;
const float MOV_SPEED = 2.0f;

// Пивот оси вращения звена 1
const float PX1 = -0.7f;
const float PY1 = 2.0f;
const float PZ1 = 0.0f;

glm::mat4 mat0 = glm::mat4(1.0f);
glm::mat4 mat1 = glm::mat4(1.0f);
glm::mat4 mat2 = glm::mat4(1.0f);
glm::mat4 mat3 = glm::mat4(1.0f);

void recalcMatrices()
{
    mat0 = glm::mat4(1.0f);

    // Звено 1: поворот вокруг Y со смещённой осью 
    glm::mat4 rot1 = glm::mat4(1.0f);
    rot1 = glm::translate(rot1, glm::vec3(PX1, PY1, PZ1));
    rot1 = glm::rotate(rot1, glm::radians(angle1), glm::vec3(0.0f, 1.0f, 0.0f));
    rot1 = glm::translate(rot1, glm::vec3(-PX1, -PY1, -PZ1));
    mat1 = mat0 * rot1;

    glm::mat4 rot2 = glm::rotate(glm::mat4(1.0f), glm::radians(angle2), glm::vec3(0.0f, 1.0f, 0.0f));
    mat2 = mat1 * rot2;

    mat3 = glm::translate(mat2, glm::vec3(0.0f, pos3Y, 0.0f));
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    bool changed = false;

    // Звено 1: Q/E — поворот вокруг Y
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) { angle1 += ROT_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) { angle1 -= ROT_SPEED * deltaTime; changed = true; }

    // Звено 2: R/F — поворот вокруг Y
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { angle2 += ROT_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) { angle2 -= ROT_SPEED * deltaTime; changed = true; }

    // Звено 3: V/B — смещение вверх/вниз
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) { pos3Y += MOV_SPEED * deltaTime; changed = true; }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { pos3Y -= MOV_SPEED * deltaTime; changed = true; }

    if (changed) recalcMatrices();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) { lastX = (float)xpos; lastY = (float)ypos; firstMouse = false; }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;
    xoffset *= 0.1f;
    yoffset *= 0.1f;
    yaw += xoffset;
    pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main()
{
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1024, 768, "LR 7 - Affine Transformations", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertex.txt", "fragment.txt");
    Model  ourModel("model.obj");

    recalcMatrices();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.50f);
        ourShader.setFloat("material.shininess", 32.0f);

        ourShader.setVec3("viewPos", cameraPos);

        glm::mat4 globalScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));

        for (unsigned int i = 0; i < ourModel.meshes.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            switch (i)
            {
            case 0: model = globalScale * mat0; break; // неподвижно
            case 1: model = globalScale * mat1; break; // поворот по Y 
            case 2: model = globalScale * mat2; break; // поворот по Y + зависит от звена 1
            case 3: model = globalScale * mat3; break; // смещение по Y + зависит от звена 2
            default: model = globalScale;        break;
            }

            ourShader.setMat4("model", model);
            glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            ourShader.setMat3("normalMatrix", normalMatrix);

            ourModel.meshes[i].Draw();
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}