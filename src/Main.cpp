#include "Hittable.h"

// Color Utility Functions
void write_color(std::ostream &out, Color pixel_color) {
	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(255.999 * pixel_color.x) << ' '
		<< static_cast<int>(255.999 * pixel_color.y) << ' '
		<< static_cast<int>(255.999 * pixel_color.z) << '\n';
}

/*
double hit_sphere(const Point3& center, double radius, const Ray& r)
{
	Vector3 oc = r.getOrigin() - center;
	auto a = r.getDirection().getSquaredLength();
	auto half_b = oc.dotProduct(r.getDirection());
	auto c = oc.getSquaredLength() - radius * radius;
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return -1.0;
	else
		return (-half_b - sqrt(discriminant)) / a;
}
*/

Color ray_color(const Ray& r, const Hittable& world)
{
	HitRecord rec;
	if (world.hit(r, 0, infinity, rec))
	{
		return 0.5 * (rec.normal + Color(1, 1, 1));
	}

	Vector3 dir(r.getDirection().getNormalied());
	auto t = 0.5 * (dir.y + 1.0);
	return (1.0 - t)*Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 2400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	// World
	HittableList world;
	world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;
	
	auto origin = Point3(0, 0, 0);
	auto horizontal = Vector3(viewport_width, 0, 0);
	auto vertical = Vector3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);

	// Render
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			auto u = double(i) / (image_width - 1);
			auto v = double(j) / (image_height - 1);
			Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			Color pixel_color = ray_color(r, world);
			write_color(std::cout, pixel_color);
		}
	}

	std::cerr << "\nDone.\n";

	return 0;
}