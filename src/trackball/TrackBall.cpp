#include "TrackBall.hpp"
#include "glm/ext/matrix_transform.hpp"

glm::mat4 TrackballCamera::getViewMatrix() const
{
    glm::mat4 viewMatrix = glm::translate(glm::mat4(1.f), glm::vec3(m_vCameraOrigin, -m_fDistance));
    viewMatrix           = glm::rotate(viewMatrix, glm::radians(m_fAngleX), glm::vec3(1.f, 0.f, 0.f));
    viewMatrix           = glm::rotate(viewMatrix, glm::radians(m_fAngleY), glm::vec3(0.f, 1.f, 0.f));
    return viewMatrix;
}

glm::vec3 TrackballCamera::getPosition() const
{
    glm::vec4 position = glm::inverse(getViewMatrix()) * glm::vec4(0.f, 0.f, 0.f, 1.f);
    return glm::vec3(position.x, position.y, position.z);
}