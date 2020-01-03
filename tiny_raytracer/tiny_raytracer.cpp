// tiny_raytracer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <limits>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_JPEG_HIGH_QULITY 100

const int width = 1024;
const int height = 768;
const int fov =  3.14f / 2.f;

struct Material
{
	Material(const Vec2f& albedo, const Vec3f& diffuse_color, float specular_exponent) : albedo(albedo), diffuse_color(diffuse_color), specular_exponent(specular_exponent){}
	Material():albedo(), diffuse_color(), specular_exponent(){}
	Vec2f albedo;
	Vec3f diffuse_color;
	float specular_exponent;
};

struct Sphere
{
	Vec3f center;
	float radius;
	Material material;

	Sphere(const Vec3f& c, float r, const Material& m) : center(c), radius(r), material(m){}

	bool ray_intersect(const Vec3f& p, const Vec3f& dir, float& t0) const 
	{
		Vec3f pc = center - p;
		float tca = pc * dir;
		float d2 = pc * pc - tca * tca;
		if (d2 > radius * radius) return false; // 垂线大于半径，必不相交
		float thc = sqrt(radius * radius - d2);
		t0 = tca - thc;
		float t1 = tca + thc;
		if (t0 < 0.f) t0 = t1;
		if (t0 < 0.f) return false;
		return true;
	}
};

struct Light
{
	Light(const Vec3f& position, float intensity) : position(position), intensity(intensity) {}

	Vec3f position;
	float intensity;
};

Vec3f reflect(const Vec3f& I, const Vec3f& N)
{
	return I - N * 2.0f * (I * N);
}

bool scene_intersect(const Vec3f& origin, const Vec3f& dir, const std::vector<Sphere>& spheres, Vec3f& point, Vec3f& N, Material& material)
{
	float spheres_dist = std::numeric_limits<float>::max();
	for (int i = 0; i < spheres.size(); i++)
	{
		float dist_i = 0.0f;
		if (spheres[i].ray_intersect(origin, dir, dist_i) && dist_i < spheres_dist)
		{
			spheres_dist = dist_i;
			point = origin + dir * dist_i;
			N = (point - spheres[i].center).normalize();
			material = spheres[i].material;
		}
	}
	return spheres_dist < 1000.0f;
}

Vec3f cast_ray(const Vec3f& origin, const Vec3f& dir, const std::vector<Sphere>& spheres, const std::vector<Light>& lights)
{
	Vec3f point, N;
	Material material;

	if (!scene_intersect(origin, dir, spheres, point, N, material))
	{
		return Vec3f(0.2f, 0.7f, 0.8f);
	}
	float diffuse_intensity = 0.f, specular_intensity = 0.0f;
	for (int i = 0; i < lights.size(); i++)
	{
		Vec3f light_dir = (lights[i].position - point).normalize();
		float light_diffuse = std::max(0.0f, light_dir* N);
		diffuse_intensity += lights[i].intensity * light_diffuse;
		specular_intensity += powf(std::max(0.f, -reflect(-light_dir, N) * dir), material.specular_exponent) * lights[i].intensity;
	}

	return material.diffuse_color * diffuse_intensity * material.albedo[0] + Vec3f(1.0f,1.0f,1.0f)* specular_intensity * material.albedo[1];
}


void out_jpg(const std::vector<Vec3f>& framebuffer)
{
	unsigned char* data = (unsigned char*)malloc(width * height * 3);
	for (size_t i = 0; i < height * width; ++i)
	{
		for (size_t j = 0; j < 3; j++)
		{
			data[3 * i + j] = (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
		}
	}

	stbi_write_jpg("./out.jpg", width, height, 3, data, STB_JPEG_HIGH_QULITY);

	free(data);
}

void render(const std::vector<Sphere>& spheres, const std::vector<Light>& lights)
{
	std::vector<Vec3f> framebuffer(width * height);

	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			float x = ((float)i / (float)width * 2.0f - 1.0f) * tan(fov / 2.0f) * width / (float)height;
			float y = -((float)j / (float)height * 2.0f - 1.0f) * tan(fov / 2.0f);
			Vec3f dir = Vec3f(x, y, -1.0f).normalize();
			framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, spheres, lights);
		}
	}

	out_jpg(framebuffer);
}

int main()
{
	Material      ivory(Vec2f(0.6, 0.3), Vec3f(0.4, 0.4, 0.3), 50.);
	Material red_rubber(Vec2f(0.9, 0.1), Vec3f(0.3, 0.1, 0.1), 10.);

	std::vector<Light>  lights;
	lights.push_back(Light(Vec3f(-20, 20, 20), 1.5));
	lights.push_back(Light(Vec3f(30, 50, -25), 1.8));
	lights.push_back(Light(Vec3f(30, 20, 30), 1.7));
	
	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(Vec3f(-3, 0, -16), 2, ivory));
	spheres.push_back(Sphere(Vec3f(-1.0, -1.5, -12), 2, red_rubber));
	spheres.push_back(Sphere(Vec3f(1.5, -0.5, -18), 3, red_rubber));
	spheres.push_back(Sphere(Vec3f(7, 5, -18), 4, ivory));
	render(spheres, lights);
	return 0;
}
