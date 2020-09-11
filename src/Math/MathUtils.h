#ifndef MATHUTILS_H
#define MATHUTILS_H

// #include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
typedef unsigned int uint;

// Constants
const double tolerance = 1e-5;
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Random Number Utilities
inline double random_double()
{
	// Returns a random real in [0,1).
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double random_double(double min, double max)
{
	// Returns a random real in [min,max).
	return min + (max - min) * random_double();
}

// Utility Functions
inline bool equal(double a, double b)
{
	return fabs(a - b) < tolerance;
}

inline double degreeToRadian(double degree)
{
	return degree * pi / 180.0;
}

inline double radianToDegree(double radian)
{
	return radian * 180.0 / pi;
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
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
