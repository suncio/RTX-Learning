#include "Hittable.h"

bool Sphere::hit(const Ray &r, const float &tmin, const float &tmax, HitRecord &rec) const
{
	Vector3 oc = r.getOrigin() - m_center;
	float a = r.getDirection().getSquaredLength();
	float half_b = oc.dotProduct(r.getDirection());
	float c = oc.getSquaredLength() - m_radius * m_radius;
	float discriminant = half_b * half_b - a * c;
	if (discriminant > 0.0)
	{
		float tmp = (-half_b - sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			rec.normal = (rec.position - m_center) / m_radius;
			return true;
		}

		tmp = (-half_b + sqrt(discriminant)) / a;
		if (tmp > tmin && tmp < tmax)
		{
			rec.t = tmp;
			rec.position = r.pointAt(rec.t);
			rec.normal = (rec.position - m_center) / m_radius;
			return true;
		}
	}
	return false;
}

bool HittableList::hit(const Ray &r, const float &tmin, const float &tmax, HitRecord &rec) const
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