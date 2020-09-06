#ifndef VECTOR3_H
#define VECTOR3_H

#include "MathUtils.h"

class Vector3
{
public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(float x0, float y0, float z0) : x(x0), y(y0), z(z0) {}
	Vector3(const float* rhs) : x(*rhs), y(*(rhs + 1)), z(*(rhs + 2)) {}
	Vector3(const Vector3 &rhs) :x(rhs.x), y(rhs.y), z(rhs.z) {}
	~Vector3() = default;

	// setter, getter
	void set(float newX, float newY, float newZ) { x = newX; y = newY; z = newZ; }
	void setX(float newX) { x = newX; }
	void setY(float newY) { y = newY; }
	void setZ(float newZ) { z = newZ; }
	inline float getX() const { return x; }
	inline float getY() const { return y; }
	inline float getZ() const { return z; }

	// normalization
	void normalize();
	Vector3 getNormalied() const;

	// length calculation
	inline float getLength() const { return static_cast<float>(sqrt(x * x + y * y + z * z)); }
	inline float getSquaredLength() const { return x * x + y * y + z * z; }

	// product
	inline float dotProduct(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
	inline Vector3 crossProduct(const Vector3& rhs) const
	{
		return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - rhs.x);
	}

	// linear interpolation
	// todo

	// overloaded operators
	inline Vector3 operator+(const Vector3 &rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	inline Vector3 operator-(const Vector3 &rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	inline Vector3 operator*(const float rhs) const { return Vector3(x*rhs, y*rhs, z*rhs); }
	inline friend Vector3 operator*(const float lhs, const Vector3& rhs) { return Vector3(rhs.x*lhs, rhs.y*lhs, rhs.z*lhs); }
	inline Vector3 operator*(const Vector3 &rhs)const { return Vector3(x*rhs.x, y*rhs.y, z*rhs.z); }
	inline Vector3 operator/(const float rhs) const { return (equal(rhs, 0.0f)) ? Vector3(0.0f, 0.0f, 0.0f) : Vector3(x / rhs, y / rhs, z / rhs); }
	inline Vector3 operator/(const Vector3 &rhs)const { return Vector3(x / rhs.x, y / rhs.y, z / rhs.z); }

	void operator+=(const Vector3 &rhs) { x += rhs.x; y += rhs.y; z += rhs.z; }
	void operator-=(const Vector3 &rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; }
	void operator*=(const float rhs) { x *= rhs; y *= rhs; z *= rhs; }
	// void operator*=(const Vector3 &rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; }
	void operator/=(const float rhs) { if (!equal(rhs, 0.0f)) { x /= rhs; y /= rhs; z /= rhs; } }
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

public:
	float x, y, z;
};

// Type aliases for vec3
using Point3 = Vector3;   // 3D point
using Color = Vector3;    // RGB color

#endif // !VECTOR3_H
