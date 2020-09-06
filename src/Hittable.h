#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"

#include <vector>
#include <memory>

struct HitRecord
{
	Point3 position;
	Vector3 normal;
	float t;
};

class Hittable
{
public:
	Hittable() = default;
	virtual ~Hittable() = default;
	virtual bool hit(const Ray& r, const float& t_min, const float& t_max, HitRecord& rec) const = 0;

};

class Sphere : public Hittable
{
public:
	Sphere(){}
	Sphere(Point3 center, float r) : m_center(center), m_radius(r) {}

	virtual bool hit(const Ray& r, const float& tmin, const float& tmax, HitRecord& rec) const override;

public:
	Point3 m_center;
	float m_radius;
	// unsigned int m_material;
};

class HittableList : public Hittable
{
public:
	HittableList() = default;
	
	void clear() { m_list.clear(); }
	void add(std::shared_ptr<Hittable> object) { m_list.push_back(object); }
	bool isEmpty() const { return m_list.empty(); }

	virtual bool hit(const Ray& r, const float& tmin, const float& tmax, HitRecord& rec) const override;

private:
	std::vector<std::shared_ptr<Hittable>> m_list;
};

#endif // !HITTABLE_H
