#include "Material.h"
#include "Camera.h"

// Color Utility Functions
void write_color(std::ostream &out, Color pixel_color, int samples_per_pixel) {
	auto r = pixel_color.x;
	auto g = pixel_color.y;
	auto b = pixel_color.z;

	// Replace NaN components with zero.
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	// Divide the color by the number of samples
	// Gamma-correct for gamma=2.0
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	// Write the translated [0,255] value of each color component.
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
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

Color ray_color(const Ray& r, const Hittable& world, int depth)
{
	HitRecord rec;

	if (depth <= 0)
		return Color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec))
	{
		// return 0.5 * (rec.normal + Color(1, 1, 1));
		// Point3 target = rec.position + rec.normal + Vector3::randomInUnitSphere();
		// Point3 target = rec.position + rec.normal + Vector3::randomUnitVector();
		// return 0.5 * ray_color(Ray(rec.position, target - rec.position), world, depth - 1);

		Ray scattered;
		Color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return Color(0, 0, 0);
	}

	Vector3 unit_direction = r.getDirection().getNormalied();
	auto t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.9, 1.0);
}

HittableList random_scene()
{
	HittableList world;

	// auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
	// world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));
	auto checker = make_shared<CheckerTexture>(Color(0.2, 0.5, 0.3), Color(0.9, 0.9, 0.3));
	world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(checker)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - Point3(4, 0.2, 0)).getLength() > 0.9) {
				shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = Color::random() * Color::random();
					sphere_material = make_shared<Lambertian>(albedo);
					auto center2 = center + Vector3(0, random_double(0, 0.5), 0);
					world.add(make_shared<MovingSphere>(center, center2, 0.0, 1.0, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = Color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<Metal>(albedo, fuzz);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<Dielectric>(1.5);
					world.add(make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<Dielectric>(1.5);
	world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
	world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

	return world;
}

HittableList two_spheres()
{
	HittableList objects;

	auto checker = make_shared<CheckerTexture>(Color(0.2, 0.5, 0.3), Color(0.9, 0.9, 0.3));

	objects.add(make_shared<Sphere>(Point3(0, -10, 0), 10, make_shared<Lambertian>(checker)));
	objects.add(make_shared<Sphere>(Point3(0,  10, 0), 10, make_shared<Lambertian>(checker)));

	return objects;
}

HittableList earth()
{
	auto earth_texture = make_shared<ImageTexture>("../RayTracer/res/earthmap.jpg");
	auto earth_surface = make_shared<Lambertian>(earth_texture);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);
	
	return HittableList(globe);
}

int main()
{
	// Image
	const auto aspect_ratio = 16.0 / 9.0;
	const int image_width = 1600;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 5;
	const int max_depth = 50;

	// World
	HittableList world;

	Point3 lookfrom;
	Point3 lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;

	switch (0)
	{
	case 1:
		world = random_scene();
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		aperture = 0.1;
		break;

	case 2:
		world = two_spheres();
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		break;

	default:
	case 4:
		world = earth();
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		break;
	}

	Vector3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;

	Camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
	/*
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;
	
	auto origin = Point3(0, 0, 0);
	auto horizontal = Vector3(viewport_width, 0, 0);
	auto vertical = Vector3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);
	*/


	// Render
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			Color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				Ray r = cam.getRay(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "\nDone.\n";

	return 0;
}