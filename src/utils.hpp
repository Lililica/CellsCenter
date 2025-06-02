#pragma once

#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/program.hpp"
#include "trackball/TrackBall.hpp"

void button_action(GLFWwindow* window, TrackballCamera& trackball);

void draw_ball(TrackballCamera& trackball, const Sphere& sphere, glm::vec3& position, const Program& program, GLuint& vao, GLFWwindow* window);