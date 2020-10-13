#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

__device__ Vector3 randomInUnitDisk(curandState *local_rand_state)
{
	Vector3 p;
	do
	{
		p = 2.0f * Vector3(curand_uniform(local_rand_state), curand_uniform(local_rand_state), 0) - Vector3(1, 1, 0);
	} while (p.getSquaredLength() >= 1.0f);
	return p;
}

class Camera
{
public:
	__device__ Camera(
		Point3  lookfrom,
		Point3  lookat,
		Vector3 vup,
		float vfov,  // vertical field-of-view in degrees
		float aspect_ratio, 
		float aperture, 
		float focus_dist
	)
	{
		m_lens_radius = aperture / 2.0f;
		float theta = vfov * ((float)M_PI) / 180.0f;
		float half_height = tan(theta / 2.0f);
		float half_width = aspect_ratio * half_height;

		w = (lookfrom - lookat).getNormalied();
		u = vup.crossProduct(w).getNormalied();
		v = w.crossProduct(u);

		m_origin = lookfrom;
		m_lowerLeftCorner = m_origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		m_horizontal = 2.0f * half_width * focus_dist * u;
		m_vertical = 2.0f * half_height * focus_dist * v;
	}


	__device__ Ray getRay(double s, double t, curandState *local_rand_state) const
	{
		// Vector3 rd = m_lens_radius * Vector3::randomInUnitDisk();
		// Vector3 offset = u * rd.x + v * rd.y;
		// return Ray(
		// 	m_origin + offset,
		// 	m_lowerLeftCorner + s * m_horizontal + t * m_vertical - m_origin - offset,
		// 	random_double(m_time0, m_time1)
		// );
		Vector3 rd = m_lens_radius * randomInUnitDisk(local_rand_state);
		Vector3 offset = u * rd.x + v * rd.y;
		return Ray(
			m_origin + offset,
			m_lowerLeftCorner + s * m_horizontal + t * m_vertical - m_origin - offset
		);
	}

private:
	Point3 m_origin;
	Point3 m_lowerLeftCorner;
	Vector3 m_horizontal;
	Vector3 m_vertical;
	Vector3 u, v, w;
	float m_lens_radius;
	// double m_time0, m_time1; // shutter open/close time
};

#endif // !CAMERA_H
