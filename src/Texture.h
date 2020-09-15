#ifndef TEXTURE_H
#define TEXTURE_H

#include "Math/Vector3.h"

class Texture
{
public:
	virtual Color value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture
{
public:
	SolidColor() = default;
	SolidColor(Color c) : m_color(c) {}

	SolidColor(double r, double g, double b)
		: SolidColor(Color(r, g, b)) {}

	virtual Color value(double u, double v, const Vector3& p) const override
	{
		return m_color;
	}

private:
	Color m_color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture() {}

	CheckerTexture(shared_ptr<Texture> t0, shared_ptr<Texture> t1)
		: m_even(t0), m_odd(t1) {}

	CheckerTexture(Color c1, Color c2)
		: m_even(make_shared<SolidColor>(c1)), m_odd(make_shared<SolidColor>(c2)) {}

	virtual Color value(double u, double v, const Vector3& p) const override
	{
		double sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
		if (sines < 0)
			return m_odd->value(u, v, p);
		else
			return m_even->value(u, v, p);
	}

public:
	shared_ptr<Texture> m_odd;
	shared_ptr<Texture> m_even;
};

class ImageTexture : public Texture
{
public :
	const static int bytes_per_pixel = 3;

	// const static int bytes_per_pixel = 3;
	ImageTexture()
		: m_data(nullptr), m_width(0), m_height(0), m_channel(0) {}

	ImageTexture(const char* filename);

	~ImageTexture();

	virtual Color value(double u, double v, const Vector3& p) const override;


private:
	unsigned char *m_data;
	int m_width, m_height;
	int m_channel;
};

#endif // !TEXTURE_H
