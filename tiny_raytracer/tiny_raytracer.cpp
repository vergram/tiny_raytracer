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

struct Sphere
{
	Vec3f center;
	float radius;

	Sphere(const Vec3f& c, float r) : center(c), radius(r){}

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

Vec3f cast_ray(const Vec3f& origin, const Vec3f& dir, const Sphere& sphere)
{
	float sphere_dist = std::numeric_limits<float>::max();
	if (!sphere.ray_intersect(origin, dir, sphere_dist))
	{
		return Vec3f(0.2f, 0.7f, 0.8f);
	}
	return Vec3f(0.4f, 0.4f, 0.4f);
}

void render()
{
	std::vector<Vec3f> framebuffer(width * height);

	Sphere sphere(Vec3f(-3, 0, -16), 2);
	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			float x = ((float)i / (float)width * 2.0f - 1.0f) * tan(fov / 2.0f) * width / (float)height;
			float y = ((float)j / (float)height * 2.0f - 1.0f) * tan(fov / 2.0f);
			Vec3f dir = Vec3f(x, y, -1.0f).normalize();
			framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, sphere);
		}
	}

	out_jpg(framebuffer);
}

int main()
{
	render();
	return 0;
}
