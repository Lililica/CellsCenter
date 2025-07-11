#pragma once

#include <cmath>
#include <iostream>
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"

class TrackballCamera {
private:
    float m_fDistance;
    float m_fAngleX;
    float m_fAngleY;

    glm::vec2 m_vCameraOrigin{0.f, 0.f}; // Camera origin in the XY plane

public:
    TrackballCamera() = default;
    TrackballCamera(float distance, float angleX, float angleY)
        : m_fDistance(distance), m_fAngleX(angleX), m_fAngleY(angleY) {};
    ~TrackballCamera() = default;

    void moveFront(float delta)
    {
        delta < 0 ? (m_fDistance < 1 ? false : m_fDistance += delta)
                  : m_fDistance += delta;
    }
    void rotateLeft(float degrees) { m_fAngleY += degrees; }
    void rotateUp(float degrees)
    {
        if (m_fAngleX >= 80 && degrees > 0)
            m_fAngleX = 80;
        else if (m_fAngleX <= -50 && degrees < 0)
            m_fAngleX = -50;
        else
            m_fAngleX += degrees;
    }

    // Moove the camera on the left or right in the plane of the camera

    void moveLeft(float dist)
    {
        m_vCameraOrigin.x += dist;
    }

    void moveUp(float dist)
    {
        m_vCameraOrigin.y -= dist;
    }

    void reset_CameraOrigin()
    {
        m_vCameraOrigin = glm::vec2(0.f, 0.f);
    }

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    void      set_to(glm::vec3 pos)
    {
        m_fDistance = pos.x;
        m_fAngleX   = pos.y;
        m_fAngleY   = pos.z;
    };
};