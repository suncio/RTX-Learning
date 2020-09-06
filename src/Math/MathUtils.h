#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>
#include <iostream>
#include <limits>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
typedef unsigned int uint;

// Constants
const double tolerance = 1e-5f;
const double infinity = std::numeric_limits<double>::infinity();
const double pi = static_cast<double>(3.1415926535897932385);

// Utility Functions
inline bool equal(double a, double b)
{
	return fabs(a - b) < tolerance;
}

inline double degreeToRadian(double degree)
{
	return degree * pi / 180.0f;
}

inline double radianToDegree(double radian)
{
	return radian * 180.0f / pi;
}

template<typename T>
T max(T a, T b)
{
	return a > b ? a : b;
}

template<typename T>
T min(T a, T b)
{
	return a < b ? a : b;
}

#endif // !MATHUTILS_H
