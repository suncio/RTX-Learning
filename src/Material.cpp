#include "Material.h"

bool Lambertian::scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	Vector3 scatter_direction = rec.normal + Vector3::randomUnitVector();
	scattered = Ray(rec.position, scatter_direction, r_in.getTime());
	attenuation = m_albedo->value(rec.u, rec.v, rec.position);
	return true;
}

bool Metal::scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	Vector3 reflected = Vector3::reflect(r_in.getDirection().getNormalied(), rec.normal);
	scattered = Ray(rec.position, reflected + m_fuzz * Vector3::randomInUnitSphere());
	attenuation = m_albedo;
	return (scattered.getDirection().dotProduct(rec.normal) > 0);
}

bool Dielectric::scatter(const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered) const
{
	attenuation = Color(1.0, 1.0, 1.0);
	double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx;

	Vector3 unit_direction = r_in.getDirection().getNormalied();

	double cos_theta = fmin(rec.normal.dotProduct(-unit_direction), 1.0);
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
	if (etai_over_etat * sin_theta > 1.0)
	{
		Vector3 reflected = Vector3::reflect(unit_direction, rec.normal);
		scattered = Ray(rec.position, reflected);
		return true;
	}

	// Schlick Approximation
	double reflect_prob = schlick(cos_theta, etai_over_etat);
	if (random_double() < reflect_prob)
	{
		Vector3 reflected = Vector3::reflect(unit_direction, rec.normal);
		scattered = Ray(rec.position, reflected);
		return true;
	}

	Vector3 refracted = Vector3::refract(unit_direction, rec.normal, etai_over_etat);
	scattered = Ray(rec.position, refracted);
	return true;
}