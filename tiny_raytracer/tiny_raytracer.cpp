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

void render()
{
	std::vector<Vec3f> framebuffer(width * height);

	for (size_t j = 0; j < height; j++)
	{
		for (size_t i = 0; i < width; i++)
		{
			framebuffer[i + j * width] = Vec3f(j / float(height), i / float(width), 0);
		}
	}

	out_jpg(framebuffer);
}

int main()
{
	render();
	return 0;
}
