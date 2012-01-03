#ifndef IOPNG_H
#define IOPNG_H

#include <png.h>

namespace io
{
	class Png
	{
	private:
		unsigned long width;
		unsigned long height;
		
		FILE* fp;
		png_structp png_ptr;
		png_infop info_ptr;
		png_byte** row_pointers;
	public:
		Png(unsigned long width, unsigned long height);
		~Png();
		bool create(const char* filename);
		void write(unsigned long x, unsigned long y, unsigned char red, unsigned char gree, unsigned char blue);
		void close();
	private:
		void free();
	private:
		static const short PIXEL_SIZE = 3; /* RGB */
	};
};

#endif