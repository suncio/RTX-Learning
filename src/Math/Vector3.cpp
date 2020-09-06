#include "Vector3.h"

void Vector3::normalize()
{
	float length = getLength();
	if (length == 1.0f || length == 0.0f)
		return;
	float scalefactor = 1.0 / length;
	x *= scalefactor;
	y *= scalefactor;
	z *= scalefactor;
}

Vector3 Vector3::getNormalied() const
{
	Vector3 result(*this);
	result.normalize();
	return result;
}