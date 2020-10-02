#ifndef RAY_H
#define RAY_H

#include "Math/Vector3.h"

class Ray
{
public:
	Ray() = default;
	~Ray() = default;
	Ray(const Vector3& org, const Vector3& dir, double time = 0.0)
		: m_origin(org), m_direction(dir), m_time(time)
	{
		m_direction.normalize();
	}
	// Getter
	Vector3 getOrigin() const { return m_origin; }
	Vector3 getDirection() const { return m_direction; }
	double getTime() const { return m_time; }

	// p(t) = origin + t * dir;
	Vector3 pointAt(const double& t) const { return m_origin + m_direction * t; }

private:
	Vector3 m_origin;
	Vector3 m_direction;
	double m_time;
};

#endif // !RAY_H
