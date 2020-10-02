#ifndef CAMERA_H
#define CAMERA_H

#include "Math/Vector3.h"

class Camera
{
public:
	Camera(
		Point3  lookfrom,
		Point3  lookat,
		Vector3 vup,
		double  vfov,  // vertical field-of-view in degrees
		double  aspect_ratio,
		double  aperture,
		double  focus_dist,
		double t0 = 0, 
		double t1 = 0
	)
	{
		auto theta = degreeToRadian(vfov);
		auto h = tan(theta / 2.0);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = (lookfrom - lookat).getNormalied();
		u = vup.crossProduct(w).getNormalied();
		v = w.crossProduct(u).getNormalied();

		m_origin = lookfrom;
		m_horizontal = focus_dist * viewport_width * u;
		m_vertical = focus_dist * viewport_height * v;
		m_lowerLeftCorner = m_origin - m_horizontal / 2.0 - m_vertical / 2.0 - focus_dist * w;

		m_lens_radius = aperture / 2;
		m_time0 = t0;
		m_time1 = t1;
	}

	Ray getRay(double s, double t) const
	{
		Vector3 rd = m_lens_radius * Vector3::randomInUnitDisk();
		Vector3 offset = u * rd.x + v * rd.y;
		return Ray(
			m_origin + offset,
			m_lowerLeftCorner + s * m_horizontal + t * m_vertical - m_origin - offset,
			random_double(m_time0, m_time1)
		);
	}

private:
	Point3 m_origin;
	Point3 m_lowerLeftCorner;
	Vector3 m_horizontal;
	Vector3 m_vertical;
	Vector3 u, v, w;
	double m_lens_radius;
	double m_time0, m_time1; // shutter open/close time
};

#endif // !CAMERA_H
