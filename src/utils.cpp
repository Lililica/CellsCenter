#include "utils.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include "GLFW/glfw3.h"
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

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        trackball->moveLeft(0.5);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        trackball->moveLeft(-0.5);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        trackball->moveUp(0.5);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        trackball->moveUp(-0.5);
}

void draw_ball(TrackballCamera* trackball, const GLobject& obj, const Program& program, GLFWwindow* window)
{
    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVP;

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // std::cout << "Window size: " << width << "x" << height << std::endl;
    float scale = 0.5f; // Scale factor for the sphere

    glUniform1f(
        glGetUniformLocation(program.getGLId(), "uScale"),
        scale
    );

    glUniformMatrix4fv(
        glGetUniformLocation(program.getGLId(), "uCameraMatrix"),
        1, GL_FALSE, glm::value_ptr(trackball->getViewMatrix())
    );

    glUniform2f(
        glGetUniformLocation(program.getGLId(), "uScreenSize"),
        float(width), float(height)
    );

    float fov    = glm::radians(70.f);
    float aspect = float(width) / height;
    float near   = 0.1f;
    float far    = 10000.f;

    glUniform1f(
        glGetUniformLocation(program.getGLId(), "ufov"),
        fov
    );
    glUniform1f(
        glGetUniformLocation(program.getGLId(), "uaspect"),
        aspect
    );
    glUniform1f(
        glGetUniformLocation(program.getGLId(), "unear"),
        near
    );
    glUniform1f(
        glGetUniformLocation(program.getGLId(), "ufar"),
        far
    );

    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uMVPMatrix"),
    //     1, GL_FALSE, glm::value_ptr(MVP)
    // );
    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uMVMatrix"),
    //     1, GL_FALSE, glm::value_ptr(MVMatrix)
    // );
    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uNormalMatrix"),
    //     1, GL_FALSE, glm::value_ptr(NormalMatrix)
    // );

    obj.draw(); // Draw the sphere at the specified position

    // MVMatrix     = glm::mat4(1.0f); // Initialize the model-view matrix to identity
    // MVMatrix     = glm::translate(MVMatrix, position);
    // NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
    // MVP          = ProjMatrix * trackball->getViewMatrix() * MVMatrix;

    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uMVPMatrix"),
    //     1, GL_FALSE, glm::value_ptr(MVP)
    // );
    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uMVMatrix"),
    //     1, GL_FALSE, glm::value_ptr(MVMatrix)
    // );
    // glUniformMatrix4fv(
    //     glGetUniformLocation(program.getGLId(), "uNormalMatrix"),
    //     1, GL_FALSE, glm::value_ptr(NormalMatrix)
    // );
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

void save_text_from_pointList(const std::vector<Point>& points, const std::string& destination)
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
        file << "[" << point.first << "," << point.second << "]";
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

std::vector<Point> load_text_to_pointList(const std::string& source)
{
    std::vector<Point> points;
    std::ifstream      file(source);
    std::string        line;

    if (!file.is_open())
    {
        std::cerr << "Could not open the file: " << source << std::endl;
        return points;
    }

    while (std::getline(file, line))
    {
        if (line.substr(0, 1) == "[")
        {
            size_t pos1 = line.find('[');
            size_t pos2 = line.find(']');
            if (pos1 != std::string::npos && pos2 != std::string::npos)
            {
                std::string        pointStr = line.substr(pos1 + 1, pos2 - pos1 - 1);
                std::istringstream iss(pointStr);
                Point              point;
                char               comma; // To consume the comma
                iss >> point.first >> comma >> point.second;
                points.push_back(point);
            }
        }
    }

    file.close();
    return points;
}

void save_energies_to_csv(const std::vector<double>& energies, const std::string& destination)
{
    std::ofstream file(destination);
    if (!file.is_open())
    {
        std::cerr << "Could not open the file: " << destination << std::endl;
        return;
    }

    for (int i = 0; i < energies.size(); ++i)
    {
        if (i > 0)
        {
            file << i << "," << "\"" << energies[i] << "\"" << ",\n";
        }
    }

    file.close();
}