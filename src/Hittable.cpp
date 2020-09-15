#include "Hittable.h"

bool Sphere::hit(const Ray &r, const double &tmin, const double &tmax, HitRecord &rec) const
{
	Vector3 oc = r.getOrigin() - m_center;
	double a = r.getDirection().getSquaredLength();
	double half_b = oc.dotProduct(r.getDirection());
	double c = oc.getSquaredLength() - m_radius * m_radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant > 0.0)
	{
		double tmp = (-half_b - sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			Vector3 outward_normal = (rec.position - m_center) / m_radius;
			rec.setFaceNormal(r, outward_normal);
			Sphere::getSphereUV((rec.position - m_center) / m_radius, rec.u, rec.v);
			rec.mat_ptr = m_mat_ptr;
			return true;
		}

		tmp = (-half_b + sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			Vector3 outward_normal = (rec.position - m_center) / m_radius;
			rec.setFaceNormal(r, outward_normal);
			Sphere::getSphereUV((rec.position - m_center) / m_radius, rec.u, rec.v);
			rec.mat_ptr = m_mat_ptr;
			return true;
		}
	}
	return false;
}

bool Sphere::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = AABB(
		m_center - Vector3(m_radius, m_radius, m_radius),
		m_center + Vector3(m_radius, m_radius, m_radius));
	return true;
}

bool MovingSphere::hit(const Ray &r, const double &tmin, const double &tmax, HitRecord &rec) const
{
	Vector3 oc = r.getOrigin() - getCenter(r.getTime());
	double a = r.getDirection().getSquaredLength();
	double half_b = oc.dotProduct(r.getDirection());
	double c = oc.getSquaredLength() - m_radius * m_radius;

	double discriminant = half_b * half_b - a * c;

	if (discriminant > 0.0)
	{
		double tmp = (-half_b - sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			Vector3 outward_normal = (rec.position - getCenter(r.getTime())) / m_radius;
			rec.setFaceNormal(r, outward_normal);
			rec.mat_ptr = m_mat_ptr;
			return true;
		}

		tmp = (-half_b + sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			Vector3 outward_normal = (rec.position - getCenter(r.getTime())) / m_radius;
			rec.setFaceNormal(r, outward_normal);
			rec.mat_ptr = m_mat_ptr;
			return true;
		}
	}
	return false;
}

bool MovingSphere::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	AABB box0(
		getCenter(t0) - Vector3(m_radius, m_radius, m_radius),
		getCenter(t0) + Vector3(m_radius, m_radius, m_radius)
	);
	AABB box1(
		getCenter(t1) - Vector3(m_radius, m_radius, m_radius),
		getCenter(t1) + Vector3(m_radius, m_radius, m_radius)
	);
	outputBox = AABB::surroundingBox(box0, box1);
	return true;
}

Point3 MovingSphere::getCenter(double time) const
{
	return m_center0 + ((time - m_time0) / (m_time1 - m_time0)) * (m_center1 - m_center0);
}

bool HittableList::hit(const Ray &r, const double &tmin, const double &tmax, HitRecord &rec) const
{
	HitRecord temp_rec;
	bool hit_anything = false;
	double closest_so_far = tmax;

	for (int i = 0; i < m_list.size(); i++)
	{
		if (m_list[i]->hit(r, tmin, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

bool HittableList::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	if (m_list.empty()) return false;

	AABB tempBox;
	bool isFirstBox = true;

	for (const auto& object : m_list)
	{
		if (!object->boundingBox(t0, t1, tempBox)) return false;
		outputBox = isFirstBox ? tempBox : AABB::surroundingBox(outputBox, tempBox);
		isFirstBox = false;
	}

	return true;
}