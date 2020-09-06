#ifndef RAY_H
#define RAY_H

#include "Math/Vector3.h"

class Ray
{
public:
	Ray() = default;
	~Ray() = default;
	Ray(const Vector3& org, const Vector3& dir)
		: m_origin(org), m_direction(dir)
	{
		m_direction.normalize();
	}
	// Getter
	Vector3 getOrigin() const { return m_origin; }
	Vector3 getDirection() const { return m_direction; }

	// p(t) = origin + t * dir;
	Vector3 pointAt(const float& t) const { return m_origin + m_direction * t; }

private:
	Vector3 m_origin;
	Vector3 m_direction;
};

#endif // !RAY_H
