#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/program.hpp"
#include "trackball/TrackBall.hpp"

void button_action(GLFWwindow* window, TrackballCamera* trackball);

void draw_ball(TrackballCamera* trackball, const Sphere& sphere, glm::vec3& position, const Program& program, GLuint& vao, GLFWwindow* window);

std::vector<glm::vec3> extract_point_from_obj(const std::string& filename);

void save_text_from_vectObj(const std::vector<glm::vec3>& points, const std::string& destination);

// void cow_setup();

bool equal_double(double& a, double& b, double epsilon = 1e-6);