#pragma once

#include <array>
#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/program.hpp"
#include "trackball/TrackBall.hpp"

using Point = std::pair<float, float>; // Représente un point (x, y)

void button_action(GLFWwindow* window, TrackballCamera* trackball);

void draw_ball(TrackballCamera* trackball, const Sphere& sphere, glm::vec3& position, const Program& program, GLuint& vao, GLFWwindow* window);

std::vector<glm::vec3> extract_point_from_obj(const std::string& filename);

void save_text_from_vectObj(const std::vector<glm::vec3>& points, const std::string& destination);

// void cow_setup();

inline double determinant3x3(std::array<std::array<float, 3>, 3>& matrix)
{
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) - matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) + matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}

inline float distance(const std::pair<float, float>& p1, const std::pair<float, float>& p2)
{
    return std::sqrt(std::pow(p2.first - p1.first, 2) + std::pow(p2.second - p1.second, 2));
}

std::pair<Point, float> welzl(std::vector<Point>& points, std::vector<Point>& boundary);
