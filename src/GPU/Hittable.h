#ifndef HITTABLE_H
#define HITTABLE_H

#include "Ray.h"

class Material;

struct HitRecord
{
	Point3 position;
	Vector3 normal;
	float t;
	Material *mat_ptr;
};

class Hittable 
{
public:
	__device__ virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const = 0;
};

class Sphere : public Hittable
{
public:
	__device__ Sphere() {}

	__device__ Sphere(Point3 cen, float r, Material *m) 
		: m_center(cen), m_radius(r), m_mat_ptr(m)
	{};

	__device__ virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;

public:
	Point3 m_center;
	float m_radius;
	Material *m_mat_ptr;
};

__device__ bool Sphere::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
{
	Vector3 oc = r.getOrigin() - m_center;

	float a = r.getDirection().getSquaredLength();
	float b = oc.dotProduct(r.getDirection());
	float c = oc.getSquaredLength() - m_radius * m_radius;
	float discriminant = b * b - a * c;

	if (discriminant > 0) {
		float temp = (-b - sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.position = r.pointAt(rec.t);
			rec.normal = (rec.position - m_center) / m_radius;
			rec.mat_ptr = m_mat_ptr;
			return true;
		}

		temp = (-b + sqrt(discriminant)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.position = r.pointAt(rec.t);
			rec.normal = (rec.position - m_center) / m_radius;
			rec.mat_ptr = m_mat_ptr;
			return true;
		}
	}

	return false;
}


class HittableList : public Hittable
{
public:
	__device__ HittableList()
		: m_list(nullptr), m_list_size(0)
	{}

	__device__ HittableList(Hittable **l, int n)
		: m_list(l), m_list_size(n)
	{}

	__device__ virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const;

public:
	Hittable **m_list;
	int m_list_size;
};

__device__ bool HittableList::hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const
{
	HitRecord tempRec;
	bool hit_anything = false;
	float closest_so_far = t_max;

	for (int i = 0; i < m_list_size; i++)
	{
		if (m_list[i]->hit(r, t_min, closest_so_far, tempRec))
		{
			hit_anything = true;
			closest_so_far = tempRec.t;
			rec = tempRec;
		}
	}

	return hit_anything;
}

#endif