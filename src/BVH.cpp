#include "BVH.h"

#include <algorithm>

inline bool boxCompare(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis)
{
	AABB boxa, boxb;

	if (!a->boundingBox(0, 0, boxa) || !b->boundingBox(0, 0, boxb))
		std::cerr << "No bounding box in BVHNode constructor.\n";

	return boxa.getMin()[axis] < boxb.getMin()[axis];
}

bool boxCompareX(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return boxCompare(a, b, 0);
}

bool boxCompareY(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return boxCompare(a, b, 1);
}

bool boxCompareZ(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b)
{
	return boxCompare(a, b, 2);
}

BVHNode::BVHNode(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, double t0, double t1)
{
	int axis = random_int(0, 2);

	auto comparator = (axis == 0) ? boxCompareX
		: (axis == 1) ? boxCompareY
		: boxCompareZ;

	size_t object_span = end - start;

	if (object_span == 1)
	{
		m_left = m_right = objects[start];
	}
	else if (object_span == 2)
	{
		if (comparator(objects[start], objects[start + 1]))
		{
			m_left = objects[start];
			m_right = objects[start + 1];
		}
		else
		{
			m_left = objects[start + 1];
			m_right = objects[start];
		}
	}
	else
	{
		std::sort(objects.begin() + start, objects.begin() + end, comparator);

		auto mid = start + object_span / 2;
		m_left = make_shared<BVHNode>(objects, start, mid, t0, t1);
		m_right = make_shared<BVHNode>(objects, mid, end, t0, t1);
	}

	AABB boxLeft, boxRight;

	if (!m_left->boundingBox(t0, t1, boxLeft)
		|| !m_right->boundingBox(t0, t1, boxRight))
		std::cerr << "No bounding box in BVHNode constructor.\n";

	m_box = AABB::surroundingBox(boxLeft, boxRight);
}

bool BVHNode::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	if (!m_box.hit(r, tmin, tmax))
		return false;

	bool hit_left = m_left->hit(r, tmin, tmax, rec);
	bool hit_right = m_right->hit(r, tmin, hit_left ? rec.t : tmax, rec);

	return hit_left || hit_right;
}

bool BVHNode::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = m_box;
	return true;
}