#include "ConstantMedium.h"

bool ConstantMedium::hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const
{
	// Print occasional samples when debugging. To enable, set enableDebug true.
	const bool enableDebug = false;
	const bool debugging = enableDebug && random_double() < 0.00001;

	HitRecord rec1, rec2;

	if (!m_boundary->hit(r, -infinity, infinity, rec1))
		return false;

	if (!m_boundary->hit(r, rec1.t + 0.0001, infinity, rec1))
		return false;

	if (debugging) std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

	if (rec1.t < tmin) rec1.t = tmin;
	if (rec2.t > tmax) rec2.t = tmax;

	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)
		rec1.t = 0;

	const auto ray_length = r.getDirection().getLength();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = m_neg_inv_density * log(random_double());

	if (hit_distance > distance_inside_boundary)
		return false;

	rec.t = rec1.t + hit_distance / ray_length;
	rec.position = r.pointAt(rec.t);

	if (debugging)
	{
		std::cerr << "hit_distance = " << hit_distance << '\n'
			<< "rec.t = " << rec.t << '\n'
			<< "rec.position = " << rec.position << '\n';
	}

	rec.normal = Vector3(1, 0, 0);	// arbitrary
	rec.front_face = true;			// also arbitrary
	rec.mat_ptr = m_phase_function;

	return true;
}

bool ConstantMedium::boundingBox(const double t0, const double t1, AABB& outputBox) const
{
	return m_boundary->boundingBox(t0, t1, outputBox);
}
