#ifndef HITTABLE_H
#define HITTABLE_H

#include <vector>
#include <memory>

#include "AABB.h"

class Material;

struct HitRecord
{
	Point3 position;
	Vector3 normal;
	shared_ptr<Material> mat_ptr;
	double t;
	double u;
	double v;
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
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const = 0;
};

class Sphere : public Hittable
{
public:
	Sphere(){}
	Sphere(Point3 center, double r, shared_ptr<Material> m) : m_center(center), m_radius(r), m_mat_ptr(m) {}

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

	static void getSphereUV(const Vector3& p, double& u, double& v)
	{
		float phi = atan2(p.z, p.x);
		float theta = asin(p.y);
		u = 1 - (phi + pi) / (2 * pi);
		v = (theta + pi / 2) / pi;
	}

public:
	Point3 m_center;
	double m_radius;
	shared_ptr<Material> m_mat_ptr;

	// unsigned int m_material;
};

class MovingSphere : public Hittable
{
public:
	MovingSphere(){}
	MovingSphere(Point3 cen0, Point3 cen1, double t0, double t1, double r, shared_ptr<Material> m)
		: m_center0(cen0), m_center1(cen1), m_time0(t0), m_time1(t1), m_radius(r), m_mat_ptr(m)
	{};

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;
	
	Point3 getCenter(double time) const;

public:
	Point3 m_center0, m_center1;
	double m_time0, m_time1;
	double m_radius;
	shared_ptr<Material> m_mat_ptr;
};

class HittableList : public Hittable
{
public:
	HittableList() = default;
	HittableList(shared_ptr<Hittable> object) { add(object); }
	
	void clear() { m_list.clear(); }
	void add(std::shared_ptr<Hittable> object) { m_list.push_back(object); }
	bool isEmpty() const { return m_list.empty(); }

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

private:
	std::vector<std::shared_ptr<Hittable>> m_list;
};

#endif // !HITTABLE_H
