#include "Material.h"
#include "Camera.h"
#include "ConstantMedium.h"
#include "BVH.h"

#include <tbb/parallel_for.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "External/stb_image_write.h"

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

void write_color(unsigned char* image_buffer, unsigned int x, unsigned int y, unsigned int width, Color pixel_color, int samples_per_pixel)
{
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
	unsigned int index = (y * width + x) * 4;
	image_buffer[index + 0] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
	image_buffer[index + 1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
	image_buffer[index + 2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
	image_buffer[index + 3] = static_cast<unsigned char>(256 * clamp(1, 0.0, 0.999));
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

Color ray_color(const Ray& r, const Color& background, const Hittable& world, int depth)
{
	HitRecord rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return Color(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	Ray scattered;
	Color attenuation;
	Color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.position);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
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

HittableList two_perlin_spheres()
{
	HittableList objects;

	auto pertext = make_shared<NoiseTexture>(4);
	objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
	objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

	return objects;
}

HittableList earth()
{
	auto earth_texture = make_shared<ImageTexture>("../RayTracer/res/earthmap.jpg");
	auto earth_surface = make_shared<Lambertian>(earth_texture);
	auto globe = make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);
	
	return HittableList(globe);
}

HittableList simple_light()
{
	HittableList objects;

	auto pertext = make_shared<NoiseTexture>(4);
	objects.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, make_shared<Lambertian>(pertext)));
	objects.add(make_shared<Sphere>(Point3(0, 2, 0), 2, make_shared<Lambertian>(pertext)));

	auto difflight = make_shared<DiffuseLight>(Color(4, 4, 4));
	objects.add(make_shared<XYRect>(3, 5, 1, 3, -2, difflight));

	return objects;
}

HittableList cornell_box()
{
	HittableList objects;

	auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = make_shared<Lambertian>(Color(.12, .45, .15));
	auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

	objects.add(make_shared<YZRect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<XZRect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<XYRect>(0, 555, 0, 555, 555, white));

	shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<RotateY>(box1, 15);
	box1 = make_shared<Translate>(box1, Vector3(265, 0, 295));
	objects.add(box1);

	shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vector3(130, 0, 65));
	objects.add(box2);

	return objects;
}

HittableList cornell_smoke()
{
	HittableList objects;

	auto red = make_shared<Lambertian>(Color(.65, .05, .05));
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	auto green = make_shared<Lambertian>(Color(.12, .45, .15));
	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

	objects.add(make_shared<YZRect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<XZRect>(213, 343, 227, 332, 554, light));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<XYRect>(0, 555, 0, 555, 555, white));

	shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
	box1 = make_shared<RotateY>(box1, 15);
	box1 = make_shared<Translate>(box1, Vector3(265, 0, 295));

	shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
	box2 = make_shared<RotateY>(box2, -18);
	box2 = make_shared<Translate>(box2, Vector3(130, 0, 65));

	objects.add(make_shared<ConstantMedium>(box1, 0.01, Color(0, 0, 0)));
	objects.add(make_shared<ConstantMedium>(box2, 0.01, Color(1, 1, 1)));

	return objects;
}

HittableList final_scene()
{
	HittableList boxes1;
	auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++)
	{
		for (int j = 0; j < boxes_per_side; j++)
		{
			auto w = 100.0;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto z1 = z0 + w;
			auto y1 = random_double(1, 101);

			boxes1.add(make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
		}
	}

	HittableList objects;

	objects.add(make_shared<BVHNode>(boxes1, 0, 1));

	auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
	objects.add(make_shared<XZRect>(123, 423, 147, 412, 554, light));
	
	auto center1 = Point3(400, 400, 200);
	auto center2 = center1 + Vector3(30, 0, 0);
	auto moving_sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
	objects.add(make_shared<MovingSphere>(center1, center2, 0, 1, 50, moving_sphere_material));

	objects.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
	objects.add(make_shared<Sphere>(Point3(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 10.0)));

	auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
	boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
	objects.add(make_shared<ConstantMedium>(boundary, .0001, Color(1, 1, 1)));

	auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("../../RayTracer/res/earthmap.jpg"));
	objects.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));
	auto pertext = make_shared<NoiseTexture>(0.1);
	objects.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

	HittableList boxes2;
	auto white = make_shared<Lambertian>(Color(.73, .73, .73));
	int ns = 1000;
	for (int j = 0; j < ns; j++)
	{
		boxes2.add(make_shared<Sphere>(Vector3::random(0, 165), 10, white));
	}

	objects.add(make_shared<Translate>(
		make_shared<RotateY>(
			make_shared<BVHNode>(boxes2, 0.0, 1.0), 15),
		Vector3(-100, 270, 395)
		)
	);
	
	return objects;
}

int main()
{
	// Image
	auto aspect_ratio = 16.0 / 9.0;
	int image_width = 1600;
	int image_height = static_cast<int>(image_width / aspect_ratio);
	int samples_per_pixel = 5;
	const int max_depth = 50;

	// World
	HittableList world;

	Point3 lookfrom;
	Point3 lookat;
	auto vfov = 40.0;
	auto aperture = 0.0;
	Color background(0, 0, 0);

	switch (0)
	{
	case 1:
		world = random_scene();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		aperture = 0.1;
		break;

	case 2:
		world = two_spheres();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		break;

	case 3:
		world = two_perlin_spheres();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		break;

	case 4:
		world = earth();
		background = Color(0.70, 0.80, 1.00);
		lookfrom = Point3(13, 2, 3);
		lookat = Point3(0, 0, 0);
		vfov = 20.0;
		break;

	case 5:
		world = simple_light();
		samples_per_pixel = 400;
		background = Color(0.0, 0.0, 0.0);
		lookfrom = Point3(26, 3, 6);
		lookat = Point3(0, 2, 0);
		vfov = 20.0;
		break;

	case 6:
		world = cornell_box();
		aspect_ratio = 1.0;
		image_width = 1200;
		image_height = 1200;
		samples_per_pixel = 200;
		background = Color(0, 0, 0);
		lookfrom = Point3(278, 278, -800);
		lookat = Point3(278, 278, 0);
		vfov = 40.0;
		break;

	case 7:
		world = cornell_smoke();
		aspect_ratio = 1.0;
		image_width = 1200;
		image_height = 1200;
		samples_per_pixel = 200;
		background = Color(0, 0, 0);
		lookfrom = Point3(278, 278, -800);
		lookat = Point3(278, 278, 0);
		vfov = 40.0;
		break;

	default:
	case 8:
		world = final_scene();
		aspect_ratio = 1.0;
		image_width = 1200;
		image_height = 1200;
		samples_per_pixel = 10000;
		background = Color(0, 0, 0);
		lookfrom = Point3(478, 278, -600);
		lookat = Point3(278, 278, 0);
		vfov = 40.0;
		break;
	}

	Vector3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;

	Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
	/*
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;
	
	auto origin = Point3(0, 0, 0);
	auto horizontal = Vector3(viewport_width, 0, 0);
	auto vertical = Vector3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3(0, 0, focal_length);
	*/


	// Serial Render
	/*
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
				pixel_color += ray_color(r, background, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	*/
	
	size_t remain = image_height * image_width;
	//std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	unsigned char* buffer = new unsigned char[image_width * image_height * 4];
	tbb::parallel_for(tbb::blocked_range<size_t>(0, image_height * image_width, 10000), [&](tbb::blocked_range<size_t>& r)
	{
		for (size_t iter = r.begin(); iter != r.end(); iter++)
		{
			remain--;
			std::cerr << "\rScanlines remaining: " << remain << ' ' << std::flush;
			Color pixel_color(0, 0, 0);
			size_t i = iter % image_width;
			size_t j = iter / image_width;
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				Ray r = cam.getRay(u, v);
				pixel_color += ray_color(r, background, world, max_depth);
			}
			write_color(buffer, i, j, image_width, pixel_color, samples_per_pixel);
		}
	}, tbb::auto_partitioner()
	);

	stbi_flip_vertically_on_write(1);
	stbi_write_png(
		"./final_scene_parallel_fix.png",
		image_width,
		image_height,
		4,
		buffer,
		image_width * 4
	);
	
	std::cerr << "\nDone.\n";

	return 0;
}