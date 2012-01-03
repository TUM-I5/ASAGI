#include <stdio.h>

#include "png.h"

io::Png::Png(unsigned long width, unsigned long height)
{
	this->width = width;
	this->height = height;
	
	fp = 0L;
	png_ptr = 0L;
	info_ptr = 0L;
	row_pointers = 0L;
}

io::Png::~Png()
{
	free();
}

bool io::Png::create(const char* filename)
{
	fp = fopen(filename, "wb");
	if (!fp)
		return false;
	
	png_ptr = png_create_write_struct
		(PNG_LIBPNG_VER_STRING, 0L, 0L, 0L);
	if (!png_ptr)
		return false;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp) 0L);
		return false;
	}
	
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		return false;
	}
	
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	
	png_init_io(png_ptr, fp);
	
	png_write_info(png_ptr, info_ptr);
	
	row_pointers = static_cast<png_byte**>(png_malloc(png_ptr, height * sizeof(png_byte*)));
	for (unsigned int i = 0; i < height; i++)
		row_pointers[i] = static_cast<png_byte*>(png_malloc(png_ptr, width * PIXEL_SIZE));
	
	return true;
}

void io::Png::write(unsigned long x, unsigned long y, unsigned char red, unsigned char green, unsigned char blue)
{
	row_pointers[y][x * PIXEL_SIZE] = red;
	row_pointers[y][x * PIXEL_SIZE + 1] = green;
	row_pointers[y][x * PIXEL_SIZE + 2] = blue;
}

void io::Png::close()
{
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0L);
	
	free();
}

void io::Png::free()
{
	if (row_pointers) {
		for (unsigned int i = 0; i < height; i++)
			png_free(png_ptr, row_pointers[i]);
	}
	png_free(png_ptr, row_pointers);
	row_pointers = 0L;
	
	if (png_ptr)
		png_destroy_write_struct(&png_ptr, &info_ptr);
	png_ptr = 0L;
	info_ptr = 0L;
	
	if (fp)
		fclose(fp);
	fp = 0L;
}