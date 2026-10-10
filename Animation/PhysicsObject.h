#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include "../Common/Types.h"

class PhysicsObject
{
private:
    Point2D m_position;
    float   m_ballRadius;
    float   m_vx;
    float   m_vy;
    float   m_radiusGrowthSpeed;
    float   m_minRadius;
    float   m_maxRadius;

public:
    PhysicsObject()
        : m_position(100.0f, 100.0f), m_ballRadius(30.0f),
          m_vx(4.0f), m_vy(3.0f),
          m_radiusGrowthSpeed(0.2f), m_minRadius(15.0f), m_maxRadius(70.0f) {}

    void Update(float deltaTime, float width, float height)
    {
        float speedFactor = deltaTime * 60.0f;

        m_position.x += m_vx * speedFactor;
        m_position.y += m_vy * speedFactor;

        m_ballRadius += m_radiusGrowthSpeed * speedFactor;
        if (m_ballRadius >= m_maxRadius || m_ballRadius <= m_minRadius)
        {
            m_radiusGrowthSpeed = -m_radiusGrowthSpeed;
        }

        if (m_position.x - m_ballRadius <= 0.0f)
        {
            m_position.x = m_ballRadius;
            m_vx = -m_vx;
        }
        else if (m_position.x + m_ballRadius >= width)
        {
            m_position.x = width - m_ballRadius;
            m_vx = -m_vx;
        }

        if (m_position.y - m_ballRadius <= 0.0f)
        {
            m_position.y = m_ballRadius;
            m_vy = -m_vy;
        }
        else if (m_position.y + m_ballRadius >= height)
        {
            m_position.y = height - m_ballRadius;
            m_vy = -m_vy;
        }
    }

    Point2D GetPosition() const { return m_position; }
    float GetX() const { return m_position.x; }
    float GetY() const { return m_position.y; }
    float GetRadius() const { return m_ballRadius; }
};

#endif // PHYSICS_OBJECT_H
