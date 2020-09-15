#ifndef MATERIAL_H
#define MATERIAL_H

#include "Hittable.h"
#include "Texture.h"

class Material
{
public:
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material
{
public:
	Lambertian(const Color& a) : m_albedo(make_shared<SolidColor>(a)) {}
	Lambertian(shared_ptr<Texture> a) : m_albedo(a) {}

	virtual ~Lambertian() = default;

	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;

private:
	shared_ptr<Texture> m_albedo;
};

class Metal : public Material
{
public:
	Metal(const Color& a, double f) : m_albedo(a), m_fuzz(f < 1 ? f : 1) {}
	virtual ~Metal() = default;

	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;

public:
	Color m_albedo;
	double m_fuzz;
};

class Dielectric : public Material
{
public:
	Dielectric(double ri) : ref_idx(ri) {}
	virtual ~Dielectric() = default;

	virtual bool scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const override;

private:
	double ref_idx;

	double schlick(double cosine, double ref_idx) const
	{
		double r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
	}
};

#endif // !MATERIAL_H