#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

bool Shader::compile()
{
    glCompileShader(m_nGLId);
    GLint status;
    glGetShaderiv(m_nGLId, GL_COMPILE_STATUS, &status);
    return status == GL_TRUE;
}

const std::string Shader::getInfoLog() const
{
    GLint length;
    glGetShaderiv(m_nGLId, GL_INFO_LOG_LENGTH, &length);
    char* log = new char[length];
    glGetShaderInfoLog(m_nGLId, length, 0, log);
    std::string logString(log);
    delete[] log;
    return logString;
}

Shader loadShader(GLenum type, const std::string& filepath)
{
    std::ifstream input(filepath.c_str());
    if (!input)
    {
        throw std::runtime_error("Unable to load the file " + filepath);
    }

    std::stringstream buffer;
    buffer << input.rdbuf();

    Shader shader(type);
    shader.setSource(buffer.str().c_str());

    return shader;
}