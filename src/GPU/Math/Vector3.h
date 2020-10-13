#ifndef VECTOR3_H
#define VECTOR3_H

#include <device_launch_parameters.h>
#include <cuda_runtime.h>

#include "MathUtils.h"

class Vector3
{
public:
	__host__ __device__ Vector3() : x(0), y(0), z(0) {}
	__host__ __device__ Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}
	__host__ __device__ Vector3(const float* rhs) : x(*rhs), y(*(rhs + 1)), z(*(rhs + 2)) {}
	__host__ __device__ Vector3(const Vector3 &rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}
	__host__ __device__ ~Vector3() = default;

	// setter, getter
	__host__ __device__ void set(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }
	__host__ __device__ void setX(float newX) { x = newX; }
	__host__ __device__ void setY(float newY) { y = newY; }
	__host__ __device__ void setZ(float newZ) { z = newZ; }
	__host__ __device__ inline float getX() const { return x; }
	__host__ __device__ inline float getY() const { return y; }
	__host__ __device__ inline float getZ() const { return z; }

	// normalization
	__host__ __device__ void normalize()
	{
		float length = getLength();
		if (length == 1.0f || length == 0.0f)
			return;
		float scalefactor = 1.0 / length;
		x *= scalefactor;
		y *= scalefactor;
		z *= scalefactor;
	}

	__host__ __device__ Vector3 getNormalied() const
	{
		Vector3 result(*this);
		result.normalize();
		return result;
	}

	// length calculation
	__host__ __device__ inline float getLength() const { return static_cast<float>(sqrt(x * x + y * y + z * z)); }
	__host__ __device__ inline float getSquaredLength() const { return x * x + y * y + z * z; }

	// product
	__host__ __device__ inline float dotProduct(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
	__host__ __device__ inline Vector3 crossProduct(const Vector3& rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	// linear interpolation
	// todo

	// overloaded operators
	__host__ __device__ inline Vector3 operator+(const Vector3 &rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	__host__ __device__ inline Vector3 operator-(const Vector3 &rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	__host__ __device__ inline Vector3 operator*(const float rhs) const { return Vector3(x*rhs, y*rhs, z*rhs); }
	__host__ __device__ inline friend Vector3 operator*(const float lhs, const Vector3& rhs) { return Vector3(rhs.x*lhs, rhs.y*lhs, rhs.z*lhs); }
	__host__ __device__ inline Vector3 operator*(const Vector3 &rhs)const { return Vector3(x*rhs.x, y*rhs.y, z*rhs.z); }
	__host__ __device__ inline Vector3 operator/(const float rhs) const { return (rhs == 0.0) ? Vector3(0.0, 0.0, 0.0) : Vector3(x / rhs, y / rhs, z / rhs); }
	__host__ __device__ inline Vector3 operator/(const Vector3 &rhs)const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }

	__host__ __device__ void operator+=(const Vector3 &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
	__host__ __device__ void operator-=(const Vector3 &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
	__host__ __device__ void operator*=(const float rhs) { x *= rhs; y *= rhs; z *= rhs; }
	// void operator*=(const Vector3 &rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
	// __host__ __device__ void operator/=(const float rhs) { if (!equal(rhs, 0.0f)) { x /= rhs; y /= rhs; z /= rhs; } }
	// void operator/=(const Vector3 &rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; }

	__host__ __device__ Vector3 operator+() const { return * this; }
	__host__ __device__ Vector3 operator-() const { return Vector3(-x, -y, -z); }

	__host__ __device__ inline bool operator==(const Vector3& rhs) const { return (equal(x, rhs.x) && equal(y, rhs.y) && equal(z, rhs.z)); }
	__host__ __device__ inline bool operator!=(const Vector3& rhs) const { return (!equal(x, rhs.x) || !equal(y, rhs.y) || !equal(z, rhs.z)); }

	__host__ __device__ float& operator[](int index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			break;
		}
	}

	// utility 
	inline friend std::ostream& operator<<(std::ostream& os, const Vector3& v)
	{
		return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}
	/*
	inline friend std::istream& operator>>(std::istream& is, const Vector3& v)
	{
		return is >> v.x >> v.y >> v.z;
	}
	*/
	__host__ __device__ inline static Vector3 random()
	{
		return Vector3(random_float(), random_float(), random_float());
	}

	__host__ __device__ inline static Vector3 random(float min, float max)
	{
		return Vector3(random_float(min, max), random_float(min, max), random_float(min, max));
	}

	__host__ __device__ inline static Vector3 reflect(const Vector3& r, const Vector3& n)
	{
		// return ray - normal * (ray.dotProduct(normal)) * 2.0f;
		return r - n * (r.dotProduct(n)) * 2.0f;
	}

	__host__ __device__ static Vector3 refract(const Vector3& r, const Vector3& n, float etai_over_etat)
	{
		float cos_theta = fmin((-r).dotProduct(n), 1.0);
		Vector3 r_out_perp = etai_over_etat * (r + cos_theta * n);
		Vector3 r_out_para = -sqrt(fabs(1.0 - r_out_perp.getSquaredLength())) * n;
		return r_out_perp + r_out_para;
	}

	__host__ __device__ static Vector3 randomInUnitSphere()
	{
		while (true)
		{
			Vector3 p = Vector3::random(-1, 1);
			if (p.getSquaredLength() >= 1) continue;
			return p;
		}
	}

	__host__ __device__ static Vector3 randomInHemiSphere(const Vector3& normal)
	{
		Vector3 inUnitSphere = randomInUnitSphere();
		if (inUnitSphere.dotProduct(normal) > 0.0)
			return inUnitSphere;
		else
			return -inUnitSphere;
	}

	__host__ __device__ static Vector3 randomUnitVector()
	{
		float a = random_float(0, 2 * pi);
		float z = random_float(-1, 1);
		float r = sqrt(1 - z * z);
		return Vector3(r * cos(a), r * sin(a), z);
	}

	__host__ __device__ static Vector3 randomInUnitDisk()
	{
		while (true)
		{
			Vector3 p = Vector3(random_float(-1, 1), random_float(-1, 1), 0);
			if (p.getSquaredLength() >= 1) continue;
			return p;
		}
	}

public:
	float x, y, z;
};

// Type aliases for vec3
using Point3 = Vector3;   // 3D point
using Color = Vector3;    // RGB color

#endif // !VECTOR3_H
