#include <device_launch_parameters.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>

#include <iostream>
#include <time.h>
#include <float.h>

#include "Math/Vector3.h"
#include "Ray.h"
#include "Hittable.h"
#include "Camera.h"
#include "Material.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "External/stb_image_write.h"

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

// Manually limit the depth to be 50
__device__ Color ray_color(const Ray& r, Hittable **world, curandState *local_rand_state)
{
	Ray cur_ray = r;
	Vector3 cur_attenuation = Vector3(1.0f, 1.0f, 1.0f);
	int depth = 50;
	
	for (int i = 0; i < depth; i++)
	{
		HitRecord rec;
		if ((*world)->hit(cur_ray, 0.001f, FLT_MAX, rec))
		{
			Ray scattered;
			Vector3 attenuation;
			if (rec.mat_ptr->scatter(cur_ray, rec, attenuation, scattered, local_rand_state))
			{
				cur_attenuation = cur_attenuation * attenuation;
				cur_ray = scattered;
			}
			else
			{
				return Vector3(0.0f, 0.0f, 0.0f);
			}
		}
		else
		{
			Vector3 unit_direction = r.getDirection().getNormalied();
			float t = 0.5f * (unit_direction.y + 1.0f);
			Vector3 c = (1.0f - t) * Vector3(1.0, 1.0, 1.0) + t * Vector3(0.5, 0.7, 1.0);
			return cur_attenuation * c;
		}
	}
	
	return Vector3(0.0f, 0.0f, 0.0f); // exceed recursion
}

__global__ void rand_init(curandState *rand_state)
{
	if (threadIdx.x == 0 && blockIdx.x == 0)
	{
		curand_init(2020, 0, 0, rand_state);
	}
}

__global__ void render_init(int max_x, int max_y, curandState *rand_state)
{
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int j = threadIdx.y + blockIdx.y * blockDim.y;
	if ((i >= max_x) || (j >= max_y)) return;
	int pixel_index = i + j * max_x;
	// Original: Each thread gets same seed, a different sequence number, no offset
	// curand_init(1984, pixel_index, 0, &rand_state[pixel_index]);
	// BUGFIX, see Issue#2: Each thread gets different seed, same sequence for
	// performance improvement of about 2x!
	curand_init(2020 + pixel_index, 0, 0, &rand_state[pixel_index]);
}

__global__ void render(Vector3 *fb, int max_x, int max_y, int ns, Camera **cam,	Hittable **world, curandState *rand_state)
{
	int i = threadIdx.x + blockIdx.x * blockDim.x;
	int j = threadIdx.y + blockIdx.y * blockDim.y;
	if ((i >= max_x) || (j >= max_y)) return;
	int pixel_index = j * max_x + i;
	
	curandState local_rand_state = rand_state[pixel_index];
	Vector3 c(0.0f, 0.0f, 0.0f);
	for (int s = 0; s < ns; s++)
	{
		float u = float(i + curand_uniform(&local_rand_state)) / float(max_x);
		float v = float(j + curand_uniform(&local_rand_state)) / float(max_y);
		Ray r = (*cam)->getRay(u, v, &local_rand_state);
		c += ray_color(r, world, &local_rand_state);
	}
	
	rand_state[pixel_index] = local_rand_state;
	// gamma correction
	c = c / float(ns);
	c[0] = sqrt(c[0]);
	c[1] = sqrt(c[1]);
	c[2] = sqrt(c[2]);
	
	fb[pixel_index] = c;
}

#define RND (curand_uniform(&local_rand_state))

__global__ void create_world(Hittable **d_list, Hittable **d_world, Camera** d_camera, int nx, int ny, curandState *rand_state)
{
	if(threadIdx.x == 0 && blockIdx.x == 0)
	{
		curandState local_rand_state = *rand_state;

		d_list[0] = new Sphere(Vector3(0, -1000.0f, -1), 1000.0f,
								new Lambertian(Vector3(0.5f, 0.5f, 0.5f)));
		int i = 1;
		for (int a = -11; a < 11; a++)
		{
			for (int b = -11; b < 11; b++)
			{
				float choose_mat = RND;
				Vector3 center(a + RND, 0.2f, b + RND);
				if (choose_mat < 0.8f)
				{
					d_list[i++] = new Sphere(center, 0.2f,
						new Lambertian(Vector3(RND * RND, RND * RND, RND * RND)));
				}
				else if (choose_mat < 0.95f)
				{
					d_list[i++] = new Sphere(center, 0.2f,
						new Metal(Vector3(0.5f * (1.0f + RND), 0.5f * (1.0f + RND), 0.5f * (1.0f + RND)), 0.5f * RND));
				}
				else
				{
					d_list[i++] = new Sphere(center, 0.2f, new Dielectric(1.5f));
				}
			}
		}
		d_list[i++] = new Sphere(Vector3(0, 1, 0), 1.0f, new Dielectric(1.5f));
		d_list[i++] = new Sphere(Vector3(-4, 1, 0), 1.0f, new Lambertian(Vector3(0.4f, 0.2f, 0.1f)));
		d_list[i++] = new Sphere(Vector3(4, 1, 0), 1.0f, new Metal(Vector3(0.7f, 0.6f, 0.5f), 0.0f));
		
		*rand_state = local_rand_state;
		*d_world = new HittableList(d_list, 22 * 22 + 1 + 3);

		Vector3 lookfrom(13, 2, 3);
		Vector3 lookat(0, 0, 0);
		float dist_to_focus = 10.0f; (lookfrom - lookat).getLength();
		float aperture = 0.1f;
		*d_camera = new Camera(
			lookfrom,
			lookat,
			Vector3(0, 1, 0),
			30.0f,
			float(nx) / float(ny),
			aperture,
			dist_to_focus
		);
	}
}

__global__ void free_world(Hittable **d_list, Hittable **d_world, Camera** d_camera)
{
	for (int i = 0; i < 22 * 22 + 1 + 3; i++)
	{
		delete ((Sphere*)d_list[i])->m_mat_ptr;
		delete d_list[i];
	}
	delete *d_world;
	delete *d_camera;
}

int main() {
	int nx = 1200;
	int ny = 600;
	int ns = 100;
	int tx = 8;
	int ty = 8;

	std::cerr << "Rendering a " << nx << "x" << ny << " image ";
	std::cerr << "in " << tx << "x" << ty << " blocks.\n";

	int num_pixels = nx * ny;
	size_t fb_size = num_pixels * sizeof(Vector3);

	// allocate FB
	Vector3 *fb;
	checkCudaErrors(cudaMallocManaged((void **)&fb, fb_size));

	// allocate random state
	curandState *d_rand_state;
	checkCudaErrors(cudaMalloc((void**)&d_rand_state, num_pixels * sizeof(curandState)));
	curandState *d_rand_state2;
	checkCudaErrors(cudaMalloc((void**)&d_rand_state2, 1 * sizeof(curandState)));

	// we need that 2nd random state to be initialized for the world creation
	rand_init<<<1, 1>>>(d_rand_state2);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaDeviceSynchronize());

	// make our world of hitables
	Hittable **d_list;
	int num_hitables = 22 * 22 + 1 + 3;
	checkCudaErrors(cudaMalloc((void**)&d_list, num_hitables * sizeof(Hittable*)));
	Hittable **d_world; 
	checkCudaErrors(cudaMalloc((void**)&d_world, sizeof(Hittable*)));
	Camera **d_camera;
	checkCudaErrors(cudaMalloc((void **)&d_camera, sizeof(Camera *)));
	create_world<<<1, 1>>>(d_list, d_world, d_camera, nx, ny, d_rand_state2);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaDeviceSynchronize());

	clock_t start, stop;
	start = clock();

	// Render our buffer
	dim3 blocks(nx / tx + 1, ny / ty + 1);
	dim3 threads(tx, ty);

	render_init<<<blocks, threads>>>(nx, ny, d_rand_state);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaDeviceSynchronize());

	stop = clock();
	double timer_seconds_ = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer_seconds_ << " seconds to init render random.\n";
	start = clock();

	render<<<blocks, threads>>>(fb, nx, ny, ns, d_camera, d_world, d_rand_state);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaDeviceSynchronize());

	stop = clock();
	double timer_seconds = ((double)(stop - start)) / CLOCKS_PER_SEC;
	std::cerr << "took " << timer_seconds << " seconds to render the image.\n";

	// Output FB as Image using stb_image
	unsigned char *image_buffer = new unsigned char[num_pixels * 4];
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			size_t pixel_index = j * nx + i;
			image_buffer[4 * pixel_index + 0] = static_cast<unsigned char>(255.99f * clamp(fb[pixel_index].x, 0.0f, 0.999f));
			image_buffer[4 * pixel_index + 1] = static_cast<unsigned char>(255.99f * clamp(fb[pixel_index].y, 0.0f, 0.999f));
			image_buffer[4 * pixel_index + 2] = static_cast<unsigned char>(255.99f * clamp(fb[pixel_index].z, 0.0f, 0.999f));
			image_buffer[4 * pixel_index + 3] = static_cast<unsigned char>(255.99f * clamp(1                , 0.0f, 0.999f));
		}
	}

	stbi_flip_vertically_on_write(1);
	stbi_write_png(
		"./img_new.png",
		nx, 
		ny,
		4, 
		image_buffer,
		nx * 4
	);

	std::cerr << "\nDone.\n";

	// clean up
	delete[] image_buffer;
	checkCudaErrors(cudaDeviceSynchronize());
	free_world<<<1, 1>>>(d_list, d_world, d_camera);
	checkCudaErrors(cudaGetLastError());
	checkCudaErrors(cudaFree(d_world));
	checkCudaErrors(cudaFree(d_camera));
	checkCudaErrors(cudaFree(d_list));
	checkCudaErrors(cudaFree(d_rand_state));
	checkCudaErrors(cudaFree(d_rand_state2));
	checkCudaErrors(cudaFree(fb));
	
	// useful for cuda-memcheck --leak-check full
	cudaDeviceReset();
}