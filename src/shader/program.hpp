#pragma once

#define GLFW_INCLUDE_NONE
#include "shader.hpp"

class Program {
public:
    Program()
        : m_nGLId(glCreateProgram())
    {
    }

    ~Program()
    {
        glDeleteProgram(m_nGLId);
    }

    Program(Program&& rvalue)
        : m_nGLId(rvalue.m_nGLId)
    {
        rvalue.m_nGLId = 0;
    }

    Program& operator=(Program&& rvalue)
    {
        m_nGLId        = rvalue.m_nGLId;
        rvalue.m_nGLId = 0;
        return *this;
    }

    GLuint getGLId() const
    {
        return m_nGLId;
    }

    void attachShader(const Shader& shader)
    {
        glAttachShader(m_nGLId, shader.getGLId());
    }

    bool link();

    const std::string getInfoLog() const;

    void use() const
    {
        glUseProgram(m_nGLId);
    }

private:
    Program(const Program&);
    Program& operator=(const Program&);

    GLuint m_nGLId;
};

// Build a GLSL program from source code
Program buildProgram(const GLchar* vsSrc, const GLchar* fsSrc);

// Load source code from files and build a GLSL program
Program loadProgram(const std::string& vsFile, const std::string& fsFile);
