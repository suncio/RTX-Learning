#ifndef MATHUTILS_H
#define MATHUTILS_H

// #include <cstdlib>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <memory>

// Usings
using std::shared_ptr;
using std::make_shared;
using std::sqrt;
typedef unsigned int uint;

// Constants
const float tolerance = 1e-5;
const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.1415926535897932385;

// Random Number Utilities
inline float random_float()
{
	// Returns a random real in [0,1).
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float random_float(float min, float max)
{
	// Returns a random real in [min,max).
	return min + (max - min) * random_float();
}

inline int random_int(int min, int max)
{
	// Returns a random integer in [min,max].
	return static_cast<int>(random_float(min, max + 1));
}

// Utility Functions
inline bool equal(float a, float b)
{
	return fabs(a - b) < tolerance;
}

inline float degreeToRadian(float degree)
{
	return degree * pi / 180.0;
}

inline float radianToDegree(float radian)
{
	return radian * 180.0 / pi;
}

inline float clamp(float x, float min, float max)
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
