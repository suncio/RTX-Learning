#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

ImageTexture::ImageTexture(const char* filename)
{
	m_channel = bytes_per_pixel;
	auto components_per_pixel = m_channel;
	m_data = stbi_load(filename, &m_width, &m_height, &components_per_pixel, components_per_pixel);
	if (m_data == nullptr)
	{
		std::cerr << "Failed to load the image->" << filename << std::endl;
		m_width = m_height = 0;
	}
}

ImageTexture::~ImageTexture() 
{
	stbi_image_free(m_data);
}

Color ImageTexture::value(double u, double v, const Vector3& p) const
{
	// If we have no texture data, then return solid cyan as a debugging aid.
	if (m_data == nullptr)
		return Color(0, 1, 1);

	// Clamp input texture coordinates to [0,1] x [1,0]
	u = clamp(u, 0.0, 1.0);
	v = 1.0 - clamp(v, 0.0, 1.0);

	int i = static_cast<int>(u * m_width);
	int j = static_cast<int>(v * m_height);

	// Clamp integer mapping, since actual coordinates should be less than 1.0
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i >= m_width)  i = m_width - 1;
	if (j >= m_height) j = m_height - 1;

	int index = (j * m_width + i) * m_channel;
	const double color_scale = 1.0 / 255.0;
	auto pixel = m_data + index;

	return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
}