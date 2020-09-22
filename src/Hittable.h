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

class Translate : public Hittable
{
public:
	Translate(shared_ptr<Hittable> p, const Vector3& displacement)
		: m_ptr(p), m_offset(displacement) {}

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	shared_ptr<Hittable> m_ptr;
	Vector3 m_offset;
};

class RotateY : public Hittable
{
public:
	RotateY(shared_ptr<Hittable> p, double angle);

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	shared_ptr<Hittable> m_ptr;
	double m_sin_theta;
	double m_cos_theta;
	bool m_hasbox;
	AABB m_bbox;
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

class XYRect : public Hittable
{
public:
	XYRect() {}
	XYRect(double _x0, double _x1, double _y0, double _y1, double _k, shared_ptr<Material> mat)
		: m_x0(_x0), m_x1(_x1), m_y0(_y0), m_y1(_y1), m_k(_k), m_mat_ptr(mat) {};

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	double m_x0, m_x1, m_y0, m_y1, m_k;
	shared_ptr<Material> m_mat_ptr;
};

class XZRect : public Hittable
{
public:
	XZRect() {}
	XZRect(double _x0, double _x1, double _z0, double _z1, double _k, shared_ptr<Material> mat)
		: m_x0(_x0), m_x1(_x1), m_z0(_z0), m_z1(_z1), m_k(_k), m_mat_ptr(mat) {};

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	double m_x0, m_x1, m_z0, m_z1, m_k;
	shared_ptr<Material> m_mat_ptr;
};

class YZRect : public Hittable
{
public:
	YZRect() {}
	YZRect(double _y0, double _y1, double _z0, double _z1, double _k, shared_ptr<Material> mat)
		: m_y0(_y0), m_y1(_y1), m_z0(_z0), m_z1(_z1), m_k(_k), m_mat_ptr(mat) {};

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	double m_y0, m_y1, m_z0, m_z1, m_k;
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

public:
	std::vector<std::shared_ptr<Hittable>> m_list;
};

class Box : public Hittable
{
public:
	Box() {}
	Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr);

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	Point3 m_min;
	Point3 m_max;
	HittableList m_sides;
};

#endif // !HITTABLE_H
