#ifndef RAY_H
#define RAY_H

#include "Math/Vector3.h"

class Ray
{
public:
	__device__ Ray() = default;
	__device__ ~Ray() = default;
	__device__ Ray(const Vector3& org, const Vector3& dir, float time = 0.0)
		: m_origin(org), m_direction(dir), m_time(time)
	{
		m_direction.normalize();
	}
	// Getter
	__device__ Vector3 getOrigin() const { return m_origin; }
	__device__ Vector3 getDirection() const { return m_direction; }
	__device__ float getTime() const { return m_time; }

	// p(t) = origin + t * dir;
	__device__ Vector3 pointAt(const float& t) const { return m_origin + m_direction * t; }

private:
	Vector3 m_origin;
	Vector3 m_direction;
	float m_time;
};

#endif // !RAY_H
