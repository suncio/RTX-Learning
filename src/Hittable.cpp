#include "Hittable.h"

bool Translate::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	Ray moved_r(r.getOrigin() - m_offset, r.getDirection(), r.getTime());
	if (!m_ptr->hit(moved_r, tmin, tmax, rec))
		return false;

	rec.position += m_offset;
	rec.setFaceNormal(moved_r, rec.normal);

	return true;
}

bool Translate::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	if (!m_ptr->boundingBox(t0, t1, outputBox))
		return false;

	outputBox = AABB(
		outputBox.getMin() + m_offset,
		outputBox.getMax() + m_offset
	);

	return true;
}

RotateY::RotateY(shared_ptr<Hittable> p, double angle)
	: m_ptr(p)
{
	auto radians = degreeToRadian(angle);
	m_sin_theta = sin(radians);
	m_cos_theta = cos(radians);
	m_hasbox = m_ptr->boundingBox(0, 1, m_bbox);

	Point3 min( infinity,  infinity,  infinity);
	Point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				double x = i * m_bbox.getMax().x + (1 - i) * m_bbox.getMin().x;
				double y = j * m_bbox.getMax().y + (1 - j) * m_bbox.getMin().y;
				double z = k * m_bbox.getMax().z + (1 - k) * m_bbox.getMin().z;

				double newx =  m_cos_theta * x + m_sin_theta * z;
				double newz = -m_sin_theta * x + m_cos_theta * z;

				Vector3 tester(newx, y, newz);

				for (int c = 0; c < 3; c++)
				{
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	m_bbox = AABB(min, max);
}

bool RotateY::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	auto origin = r.getOrigin();
	auto direction = r.getDirection();

	origin[0] = m_cos_theta * r.getOrigin()[0] - m_sin_theta * r.getOrigin()[2];
	origin[2] = m_sin_theta * r.getOrigin()[0] + m_cos_theta * r.getOrigin()[2];

	direction[0] = m_cos_theta * r.getDirection()[0] - m_sin_theta * r.getDirection()[2];
	direction[2] = m_sin_theta * r.getDirection()[0] + m_cos_theta * r.getDirection()[2];

	Ray rotated_r(origin, direction, r.getTime());

	if (!m_ptr->hit(rotated_r, tmin, tmax, rec))
		return false;

	auto p = rec.position;
	auto normal = rec.normal;

	p[0] =  m_cos_theta * rec.position[0] + m_sin_theta * rec.position[2];
	p[2] = -m_sin_theta * rec.position[0] + m_cos_theta * rec.position[2];

	normal[0] =  m_cos_theta * rec.normal[0] + m_sin_theta * rec.normal[2];
	normal[2] = -m_sin_theta * rec.normal[0] + m_cos_theta * rec.normal[2];

	rec.position = p;
	rec.setFaceNormal(rotated_r, normal);

	return true;
}

bool RotateY::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = m_bbox;
	return m_hasbox;
}

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

bool XYRect::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	double t = (m_k - r.getOrigin().z) / r.getDirection().z;
	if (t < tmin || t > tmax)
		return false;

	double x = r.getOrigin().x + t * r.getDirection().x;
	double y = r.getOrigin().y + t * r.getDirection().y;
	if (x < m_x0 || x > m_x1 || y < m_y0 || y > m_y1)
		return false;

	rec.u = (x - m_x0) / (m_x1 - m_x0);
	rec.v = (y - m_y0) / (m_y1 - m_y0);
	rec.t = t;
	Vector3 outward_normal = Vector3(0, 0, 1);
	rec.setFaceNormal(r, outward_normal);
	rec.mat_ptr = m_mat_ptr;
	rec.position = r.pointAt(t);

	return true;
}

bool XYRect::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = AABB(Point3(m_x0, m_y0, m_k - 0.0001), Point3(m_x1, m_y1, m_k + 0.0001));
	return true;
}

bool XZRect::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	double t = (m_k - r.getOrigin().y) / r.getDirection().y;
	if (t < tmin || t > tmax)
		return false;

	double x = r.getOrigin().x + t * r.getDirection().x;
	double z = r.getOrigin().z + t * r.getDirection().z;
	if (x < m_x0 || x > m_x1 || z < m_z0 || z > m_z1)
		return false;

	rec.u = (x - m_x0) / (m_x1 - m_x0);
	rec.v = (z - m_z0) / (m_z1 - m_z0);
	rec.t = t;
	Vector3 outward_normal = Vector3(0, 1, 0);
	rec.setFaceNormal(r, outward_normal);
	rec.mat_ptr = m_mat_ptr;
	rec.position = r.pointAt(t);

	return true;
}

bool XZRect::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = AABB(Point3(m_x0, m_k - 0.0001, m_z0), Point3(m_x1, m_k + 0.0001, m_z1));
	return true;
}

bool YZRect::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	double t = (m_k - r.getOrigin().x) / r.getDirection().x;
	if (t < tmin || t > tmax)
		return false;

	double y = r.getOrigin().y + t * r.getDirection().y;
	double z = r.getOrigin().z + t * r.getDirection().z;
	if (y < m_y0 || y > m_y1 || z < m_z0 || z > m_z1)
		return false;

	rec.u = (y - m_y0) / (m_y1 - m_y0);
	rec.v = (z - m_z0) / (m_z1 - m_z0);
	rec.t = t;
	Vector3 outward_normal = Vector3(1, 0, 0);
	rec.setFaceNormal(r, outward_normal);
	rec.mat_ptr = m_mat_ptr;
	rec.position = r.pointAt(t);

	return true;
}

bool YZRect::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = AABB(Point3(m_k - 0.0001, m_y0, m_z0), Point3(m_k + 0.0001, m_y1, m_z1));
	return true;
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

Box::Box(const Point3& p0, const Point3& p1, shared_ptr<Material> ptr)
{
	m_min = p0;
	m_max = p1;

	m_sides.add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p1.z, ptr));
	m_sides.add(make_shared<XYRect>(p0.x, p1.x, p0.y, p1.y, p0.z, ptr));

	m_sides.add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p1.y, ptr));
	m_sides.add(make_shared<XZRect>(p0.x, p1.x, p0.z, p1.z, p0.y, ptr));

	m_sides.add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p1.x, ptr));
	m_sides.add(make_shared<YZRect>(p0.y, p1.y, p0.z, p1.z, p0.x, ptr));
}

bool Box::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	return m_sides.hit(r, tmin, tmax, rec);
}

bool Box::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	outputBox = AABB(m_min, m_max);
	return true;
}