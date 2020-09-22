#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "Material.h"

class ConstantMedium : public Hittable
{
public:
	ConstantMedium(shared_ptr<Hittable> b, double d, shared_ptr<Texture> a)
		: m_boundary(b),
		m_neg_inv_density(-1 / d),
		m_phase_function(make_shared<Isotropic>(a))
	{}

	ConstantMedium(shared_ptr<Hittable> b, double d, Color c)
		: m_boundary(b),
		m_neg_inv_density(-1 / d),
		m_phase_function(make_shared<Isotropic>(c))
	{}

	virtual bool hit(const Ray& r, const double& tmin, const double& tmax, HitRecord& rec) const override;
	virtual bool boundingBox(const double t0, const double t1, AABB& outputBox) const override;

public:
	shared_ptr<Hittable> m_boundary;
	double m_neg_inv_density;
	shared_ptr<Material> m_phase_function;
};

#endif // !CONSTANT_MEDIUM_H
