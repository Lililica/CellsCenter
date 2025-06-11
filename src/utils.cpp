#include "utils.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/shader.hpp"

void button_action(GLFWwindow* window, TrackballCamera* trackball)
{
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        trackball->rotateLeft(2);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        trackball->rotateLeft(-2);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        trackball->rotateUp(-2);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        trackball->rotateUp(2);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        trackball->moveFront(-0.3);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        trackball->moveFront(0.3);
}

void draw_ball(TrackballCamera* trackball, const Sphere& sphere, glm::vec3& position, const Program& program, GLuint& vao, GLFWwindow* window)
{
    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVP;

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // std::cout << "Window size: " << width << "x" << height << std::endl;

    ProjMatrix   = glm::perspective(glm::radians(70.f), float(width) / height, 0.1f, 10000.f);
    MVMatrix     = glm::translate(glm::mat4(1.), position);
    NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
    MVP          = ProjMatrix * trackball->getViewMatrix() * MVMatrix;

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
    glUniform2f(
        glGetUniformLocation(program.getGLId(), "uScreenSize"),
        float(width), float(height)
    );

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
    glBindVertexArray(0);
}

std::vector<glm::vec3> extract_point_from_obj(const std::string& filename)
{
    std::vector<glm::vec3> points;
    std::ifstream          file(filename);
    std::string            line;

    if (!file.is_open())
    {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return points;
    }

    while (std::getline(file, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream iss(line.substr(2));
            glm::vec3          point;
            iss >> point.x >> point.y >> point.z;
            points.push_back(point);
        }
    }

    file.close();
    return points;
}

void save_text_from_vectObj(const std::vector<glm::vec3>& points, const std::string& destination)
{
    // Create a txt file and write the points to it
    std::ofstream file(destination);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file: " << destination << std::endl;
        return;
    }

    file << "[";
    for (const auto& point : points)
    {
        file << "[" << point.x << "," << point.y << "," << point.z << "]";
        if (&point != &points.back()) // Check if it's not the last point
        {
            file << "," << "\n";
        }
        else
        {
            file << "]\n";
        }
    }

    file.close();
}
