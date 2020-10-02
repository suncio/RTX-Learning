#include <device_launch_parameters.h>
#include <cuda_runtime.h>

#include <iostream>
#include <time.h>

#include "Math/Vector3.h"
#include "Ray.h"

// limited version of checkCudaErrors from helper_cuda.h in CUDA examples
#define checkCudaErrors(val) check_cuda( (val), #val, __FILE__, __LINE__ )

void check_cuda(cudaError_t result, char const *const func, const char *const file, int const line) {
	if (result) {
		std::cerr << "CUDA error = " << static_cast<unsigned int>(result) << " at " <<
			file << ":" << line << " '" << func << "' \n";
		// Make sure we call CUDA Device Reset before exiting
		cudaDeviceReset();
		exit(99);
	}
}

/*
__device__ Color ray_color(const Ray& r, const Color& background, const Hittable& world, int depth)
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
*/

__device__ Color ray_color(const Ray& r)
{
	Vector3 unit_direction = r.getDirection().getNormalied();
	float t = 0.5f * (unit_direction.y + 1.0f);
	return (1.0f - t) * Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
}

__global__ void render(Vector3 *fb, int max_x, int max_y, 
	Vector3 lower_left_corner, Vector3 horizontal, Vector3 vertical, Vector3 origin)
{
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int j = threadIdx.y + blockIdx.y * blockDim.y;
	if ((i >= max_x) || (j >= max_y)) return;
	int pixel_index = j * max_x + i;
	float u = float(i) / float(max_x);
	float v = float(j) / float(max_y);
	Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
	fb[pixel_index] = ray_color(r);
}

int main() {
	int nx = 1200;
	int ny = 600;
	int tx = 8;
	int ty = 8;

	std::cerr << "Rendering a " << nx << "x" << ny << " image ";
	std::cerr << "in " << tx << "x" << ty << " blocks.\n";

	int num_pixels = nx * ny;
	size_t fb_size = num_pixels * sizeof(Vector3);

	// allocate FB
	Vector3 *fb;
	checkCudaErrors(cudaMallocManaged((void **)&fb, fb_size));

	clock_t start, stop;
	start = clock();
	// Render our buffer
	dim3 blocks(nx / tx + 1, ny / ty + 1);
	dim3 threads(tx, ty);
	render<<<blocks, threads >>>(fb, nx, ny,
		Vector3(-2.0, -1.0, -1.0),
		Vector3(4.0, 0.0, 0.0),
		Vector3(0.0, 2.0, 0.0),
		Vector3(0.0, 0.0, 0.0)
		);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaDeviceSynchronize());
	stop = clock();
	double timer_seconds = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer_seconds << " seconds.\n";

	// Output FB as Image
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			size_t pixel_index = j * nx + i;
			int ir = int(255.99 * fb[pixel_index].x);
			int ig = int(255.99 * fb[pixel_index].y);
			int ib = int(255.99 * fb[pixel_index].z);
			std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}

	checkCudaErrors(cudaFree(fb));
}