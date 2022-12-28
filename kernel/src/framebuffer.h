#pragma once 

#include <stddef.h>

struct Framebuffer
{
	void* base_adress;
	size_t buffer_size;
	unsigned int width;
	unsigned int height;
	unsigned int pixels_per_scanline;
};
