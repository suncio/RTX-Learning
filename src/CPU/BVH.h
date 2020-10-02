#ifndef BVH_H
#define BVH_H

#include "Hittable.h"

class BVHNode : public Hittable
{
public:
	BVHNode() = default;

	BVHNode(HittableList& list, double t0, double t1)
		: BVHNode(list.m_list, 0, list.m_list.size(), t0, t1)
	{}

	BVHNode(std::vector<shared_ptr<Hittable>>& objects, size_t start, size_t end, double t0, double t1);

	shared_ptr<Hittable> getLeftChild() { return m_left; }
	shared_ptr<Hittable> getRightChild() { return m_right; }

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

private:
	shared_ptr<Hittable> m_left;
	shared_ptr<Hittable> m_right;
	AABB m_box;
};

#endif // !BVH_H
