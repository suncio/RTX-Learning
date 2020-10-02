#ifndef AABB_H
#define AABB_H

#include <vector>

#include "Ray.h"
#include "Math/Vector3.h"

class AABB
{
public:
	AABB() = default;
	AABB(const Vector3& a, const Vector3& b) { m_min = a; m_max = b; }

	Vector3 getMin() const { return m_min; }
	Vector3 getMax() const { return m_max; }

	/*
	bool isInside(const Point3& point) const
	{

	}
	*/

	bool hit(const Ray& r, double tmin, double tmax) const
	{
		for (int a = 0; a < 3; a++)
		{
			double t0 = fmin(
				(getMin()[a] - r.getOrigin()[a]) / r.getDirection()[a],
				(getMax()[a] - r.getOrigin()[a]) / r.getDirection()[a]
			);
			double t1 = fmax(
				(getMin()[a] - r.getOrigin()[a]) / r.getDirection()[a],
				(getMax()[a] - r.getOrigin()[a]) / r.getDirection()[a]
			);
			tmin = fmax(t0, tmin);
			tmax = fmin(t1, tmax);
			if (tmax <= tmin)
				return false;
		}
		return true;
	}

	static AABB surroundingBox(const AABB& box0, const AABB& box1)
	{
		Vector3 small(
			fmin(box0.getMin().x, box1.getMin().x),
			fmin(box0.getMin().y, box1.getMin().y),
			fmin(box0.getMin().z, box1.getMin().z));

		Vector3 big(
			fmax(box0.getMax().x, box1.getMax().x),
			fmax(box0.getMax().y, box1.getMax().y),
			fmax(box0.getMax().z, box1.getMax().z));

		return AABB(small, big);
	}

private:
	Vector3 m_min, m_max;
};

#endif // !AABB_H
