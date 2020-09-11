#ifndef VECTOR3_H
#define VECTOR3_H

#include "MathUtils.h"

class Vector3
{
public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(double x0, double y0, double z0) : x(x0), y(y0), z(z0) {}
	Vector3(const double* rhs) : x(*rhs), y(*(rhs + 1)), z(*(rhs + 2)) {}
	Vector3(const Vector3 &rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}
	~Vector3() = default;

	// setter, getter
	void set(double newX, double newY, double newZ) { x = newX; y = newY; z = newZ; }
	void setX(double newX) { x = newX; }
	void setY(double newY) { y = newY; }
	void setZ(double newZ) { z = newZ; }
	inline double getX() const { return x; }
	inline double getY() const { return y; }
	inline double getZ() const { return z; }

	// normalization
	void normalize();
	Vector3 getNormalied() const;

	// length calculation
	inline double getLength() const { return static_cast<double>(sqrt(x * x + y * y + z * z)); }
	inline double getSquaredLength() const { return x * x + y * y + z * z; }

	// product
	inline double dotProduct(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
	inline Vector3 crossProduct(const Vector3& rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	// linear interpolation
	// todo

	// overloaded operators
	inline Vector3 operator+(const Vector3 &rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	inline Vector3 operator-(const Vector3 &rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	inline Vector3 operator*(const double rhs) const { return Vector3(x*rhs, y*rhs, z*rhs); }
	inline friend Vector3 operator*(const double lhs, const Vector3& rhs) { return Vector3(rhs.x*lhs, rhs.y*lhs, rhs.z*lhs); }
	inline Vector3 operator*(const Vector3 &rhs)const { return Vector3(x*rhs.x, y*rhs.y, z*rhs.z); }
	inline Vector3 operator/(const double rhs) const { return (rhs == 0.0) ? Vector3(0.0, 0.0, 0.0) : Vector3(x / rhs, y / rhs, z / rhs); }
	inline Vector3 operator/(const Vector3 &rhs)const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }

	void operator+=(const Vector3 &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
	void operator-=(const Vector3 &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
	void operator*=(const double rhs) { x *= rhs; y *= rhs; z *= rhs; }
	// void operator*=(const Vector3 &rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
	void operator/=(const double rhs) { if (!equal(rhs, 0.0f)) { x /= rhs; y /= rhs; z /= rhs; } }
	// void operator/=(const Vector3 &rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; }

	Vector3 operator+() const { return * this; }
	Vector3 operator-() const { return Vector3(-x, -y, -z); }

	inline bool operator==(const Vector3& rhs) const { return (equal(x, rhs.x) && equal(y, rhs.y) && equal(z, rhs.z)); }
	inline bool operator!=(const Vector3& rhs) const { return (!equal(x, rhs.x) || !equal(y, rhs.y) || !equal(z, rhs.z)); }

	// utility 
	inline friend std::ostream& operator<<(std::ostream& os, const Vector3& v) 
	{
		return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	inline static Vector3 random()
	{
		return Vector3(random_double(), random_double(), random_double());
	}

	inline static Vector3 random(double min, double max)
	{
		return Vector3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

	inline static Vector3 reflect(const Vector3& r, const Vector3& n)
	{
		// return ray - normal * (ray.dotProduct(normal)) * 2.0f;
		return r - n * (r.dotProduct(n)) * 2.0f;
	}

	static Vector3 refract(const Vector3& r, const Vector3& n, double etai_over_etat)
	{
		double cos_theta = fmin((-r).dotProduct(n), 1.0);
		Vector3 r_out_perp = etai_over_etat * (r + cos_theta * n);
		Vector3 r_out_para = -sqrt(fabs(1.0 - r_out_perp.getSquaredLength())) * n;
		return r_out_perp + r_out_para;
	}

	static Vector3 randomInUnitSphere()
	{
		while (true)
		{
			Vector3 p = Vector3::random(-1, 1);
			if (p.getSquaredLength() >= 1) continue;
			return p;
		}
	}

	static Vector3 randomInHemiSphere(const Vector3& normal)
	{
		Vector3 inUnitSphere = randomInUnitSphere();
		if (inUnitSphere.dotProduct(normal) > 0.0)
			return inUnitSphere;
		else
			return -inUnitSphere;
	}

	static Vector3 randomUnitVector()
	{
		double a = random_double(0, 2 * pi);
		double z = random_double(-1, 1);
		double r = sqrt(1 - z * z);
		return Vector3(r * cos(a), r * sin(a), z);
	}

	static Vector3 randomInUnitDisk()
	{
		while (true)
		{
			Vector3 p = Vector3(random_double(-1, 1), random_double(-1, 1), 0);
			if (p.getSquaredLength() >= 1) continue;
			return p;
		}
	}

public:
	double x, y, z;
};

// Type aliases for vec3
using Point3 = Vector3;   // 3D point
using Color = Vector3;    // RGB color

#endif // !VECTOR3_H
