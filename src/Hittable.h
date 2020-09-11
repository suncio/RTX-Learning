#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"

#include <vector>
#include <memory>

class Material;

struct HitRecord
{
	Point3 position;
	Vector3 normal;
	shared_ptr<Material> mat_ptr;
	double t;
	bool front_face;

	inline void setFaceNormal(const Ray& r, const Vector3& outward_normal)
	{
		front_face = r.getDirection().dotProduct(outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};


class Hittable
{
public:
	Hittable() = default;
	virtual ~Hittable() = default;
	virtual bool hit(const Ray& r, const double& t_min, const double& t_max, HitRecord& rec) const = 0;

};

class Sphere : public Hittable
{
public:
	Sphere(){}
	Sphere(Point3 center, double r, shared_ptr<Material> m) : m_center(center), m_radius(r), m_mat_ptr(m) {}

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;

public:
	Point3 m_center;
	double m_radius;
	shared_ptr<Material> m_mat_ptr;

	// unsigned int m_material;
};

class HittableList : public Hittable
{
public:
	HittableList() = default;
	
	void clear() { m_list.clear(); }
	void add(std::shared_ptr<Hittable> object) { m_list.push_back(object); }
	bool isEmpty() const { return m_list.empty(); }

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;

private:
	std::vector<std::shared_ptr<Hittable>> m_list;
};

#endif // !HITTABLE_H
