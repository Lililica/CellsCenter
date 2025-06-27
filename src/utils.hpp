#pragma once

#include <array>
#include "glm/gtc/type_ptr.hpp"
#include "object/sphere.hpp"
#include "shader/program.hpp"
#include "trackball/TrackBall.hpp"

// Define a list of color glm::vec3
constexpr glm::vec3 RED   = glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 BLUE  = glm::vec3(0.0f, 0.0f, 1.0f);
constexpr glm::vec3 WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLACK = glm::vec3(0.0f, 0.0f, 0.0f);

struct Vertex {
    glm::vec3 pos;    // Position of the vertex
    glm::vec3 col;    // Color of the vertex
    glm::vec3 normal; // Normal vector for lighting

    Vertex(const glm::vec3& position, const glm::vec3& color, const glm::vec3& normal_vector)
        : pos(position), col(color), normal(normal_vector) {}
};

struct GLobject {
    GLuint _vbo;
    GLuint _vao;
    GLuint _n_vertex;
    GLuint _draw_mode;
    bool   _is_cloud;

    GLobject(const std::vector<Vertex>& mesh, const GLuint draw_mode, const bool iscloud = false)
        : _n_vertex(mesh.size()), _draw_mode(draw_mode), _is_cloud(iscloud)
    {
        glGenBuffers(1, &_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        const GLuint VERTEX_ATTR_POS = 0;
        const GLuint VERTEX_ATTR_COL = 1;
        const GLuint VERTEX_ATTR_NOR = 2;

        glEnableVertexAttribArray(VERTEX_ATTR_POS);
        glEnableVertexAttribArray(VERTEX_ATTR_COL);
        glEnableVertexAttribArray(VERTEX_ATTR_NOR);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        glVertexAttribPointer(VERTEX_ATTR_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));
        glVertexAttribPointer(VERTEX_ATTR_COL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, col));
        glVertexAttribPointer(VERTEX_ATTR_NOR, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~GLobject()
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
    }

    bool
        is_cloud() const
    {
        return _is_cloud;
    }

    void free_gpu()
    {
        glDeleteVertexArrays(1, &_vao);
        glDeleteBuffers(1, &_vbo);
    }

    void set_vertex_data(const std::vector<Vertex>& mesh)
    {
        _n_vertex = mesh.size();
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void
        draw() const
    {
        glBindVertexArray(_vao);
        glDrawArrays(_draw_mode, 0, _n_vertex);
        glBindVertexArray(0);
    }
};

using Point = std::pair<float, float>; // Représente un point (x, y)

void button_action(GLFWwindow* window, TrackballCamera* trackball);

void draw_ball(TrackballCamera* trackball, const GLobject& sphere, const Program& program, GLFWwindow* window);

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

void save_text_from_pointList(const std::vector<Point>& points, const std::string& destination);

std::vector<Point> load_text_to_pointList(const std::string& source);

void save_energies_to_csv(const std::vector<double>& energies, const std::string& destination);