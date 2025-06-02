#include "utils.hpp"
#include <glm/glm.hpp>
#include <iostream>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/shader.hpp"

void button_action(GLFWwindow* window, TrackballCamera& trackball)
{
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        trackball.rotateLeft(2);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        trackball.rotateLeft(-2);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        trackball.rotateUp(-2);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        trackball.rotateUp(2);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        trackball.moveFront(-0.3);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        trackball.moveFront(0.3);
}

void draw_ball(TrackballCamera& trackball, const Sphere& sphere, glm::vec3& position, const Program& program, GLuint& vao, GLFWwindow* window)
{
    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVP;

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // std::cout << "Window size: " << width << "x" << height << std::endl;

    ProjMatrix   = glm::perspective(glm::radians(70.f), float(width) / height, 0.1f, 100.f);
    MVMatrix     = glm::translate(glm::mat4(1.), position);
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
    MVP          = ProjMatrix * trackball.getViewMatrix() * MVMatrix;

    glUniformMatrix4fv(
        glGetUniformLocation(program.getGLId(), "uMVPMatrix"),
        1, GL_FALSE, glm::value_ptr(MVP)
    );
    glUniformMatrix4fv(
        glGetUniformLocation(program.getGLId(), "uMVMatrix"),
        1, GL_FALSE, glm::value_ptr(MVMatrix)
    );
    glUniformMatrix4fv(
        glGetUniformLocation(program.getGLId(), "uNormalMatrix"),
        1, GL_FALSE, glm::value_ptr(NormalMatrix)
    );

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
}