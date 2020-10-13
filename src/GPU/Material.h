#ifndef MATERIAL_H
#define MATERIAL_H

#include <curand_kernel.h>

#include "Ray.h"
#include "Hittable.h"


__device__ float schlick(float cosine, float ref_idx)
{
	float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

__device__ bool refract(const Vector3& v, const Vector3& n, float ni_over_nt, Vector3& refracted)
{
	Vector3 uv = v.getNormalied();
	float dt = uv.dotProduct(n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}

#define RANDVEC3 Vector3(curand_uniform(local_rand_state),curand_uniform(local_rand_state),curand_uniform(local_rand_state))

__device__ Vector3 randomInUnitSphere(curandState *local_rand_state)
{
	Vector3 p;
	do
	{
		p = 2.0f * RANDVEC3 - Vector3(1, 1, 1);
	} while (p.getSquaredLength() >= 1.0f);
	return p;
}

__device__ Vector3 reflect(const Vector3& v, const Vector3& n)
{
	return v - 2.0f * v.dotProduct(n) * n;
}

class Material
{
public:
	__device__ virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered, curandState *local_rand_state) const = 0;
};

class Lambertian : public Material 
{
public:
	__device__ Lambertian(const Vector3& a) : m_albedo(a) {}
	__device__ virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered, curandState *local_rand_state) const
	{
		Vector3 target = rec.position + rec.normal + randomInUnitSphere(local_rand_state);
		scattered = Ray(rec.position, target - rec.position);
		attenuation = m_albedo;
		return true;
	}

public:
	Vector3 m_albedo;
};

class Metal : public Material
{
public:
	__device__ Metal(const Vector3& a, float f) : m_albedo(a), m_fuzz(f) {}
	__device__ virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered, curandState *local_rand_state) const
	{
		Vector3 reflected = reflect(r_in.getDirection().getNormalied(), rec.normal);
		scattered = Ray(rec.position, reflected + m_fuzz * randomInUnitSphere(local_rand_state));
		attenuation = m_albedo;
		return (scattered.getDirection().dotProduct(rec.normal) > 0.0f);
	}

public:
	Vector3 m_albedo;
	float m_fuzz;
};

class Dielectric : public Material
{
public:
	__device__ Dielectric(float ri) : ref_idx(ri) {}
	__device__ virtual bool scatter(const Ray& r_in, const HitRecord& rec, Vector3& attenuation, Ray& scattered, curandState *local_rand_state) const
	{
		Vector3 outward_normal;
		Vector3 reflected = reflect(r_in.getDirection(), rec.normal);
		float ni_over_nt;
		attenuation = Vector3(1.0f, 1.0f, 1.0f);
		Vector3 refracted;
		float reflect_prob;
		float cosine;

		if (r_in.getDirection().dotProduct(rec.normal) > 0.0f)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = r_in.getDirection().dotProduct(rec.normal) / r_in.getDirection().getLength();
			cosine = sqrt(1.0f - ref_idx * ref_idx * (1 - cosine * cosine));
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1.0f / ref_idx;
			cosine = -r_in.getDirection().dotProduct(rec.normal) / r_in.getDirection().getLength();
		}

		if (refract(r_in.getDirection(), outward_normal, ni_over_nt, refracted))
			reflect_prob = schlick(cosine, ref_idx);
		else
			reflect_prob = 1.0f;

		if (curand_uniform(local_rand_state) < reflect_prob)
			scattered = Ray(rec.position, reflected);
		else
			scattered = Ray(rec.position, refracted);

		return true;
	}

public:
	float ref_idx;
};

#endif // !MATERIAL_H
