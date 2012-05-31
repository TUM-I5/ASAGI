/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU General Public License, wie von der Free Software Foundation,
 *  Version 3 der Lizenz oder (nach Ihrer Option) jeder spaeteren
 *  veroeffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 *  Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 * @version \$Id$
 */

#include "pngwriter.h"

#include <cstdio>

/**
 * @param width The width of the new png file
 * @param height The height of the new png file
 */
io::PngWriter::PngWriter(unsigned long width, unsigned long height)
	: width(width),
	height(height)
{
	fp = 0L;
	png_ptr = 0L;
	info_ptr = 0L;
	row_pointers = 0L;
}

/**
 * Create the png file and allocate the buffer for the pixels
 * 
 * @param filename The name for the png file
 * @return <code>true</code> on success, <code>false</code> otherwise
 */
bool io::PngWriter::create(const char* filename)
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
		fclose(fp);
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

/**
 * Set a single pixel in png buffer
 */
void io::PngWriter::write(unsigned long x, unsigned long y, unsigned char red,
	unsigned char green, unsigned char blue)
{
	if (y >= height)
		return;
	if (x >= width)
		return;
	
	row_pointers[y][x * PIXEL_SIZE] = red;
	row_pointers[y][x * PIXEL_SIZE + 1] = green;
	row_pointers[y][x * PIXEL_SIZE + 2] = blue;
}

/**
 * Write the buffer to the file handle, close the png file and free all memory
 */
void io::PngWriter::close()
{
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, 0L);
	
	free();
}

/**
 * Free all allocated memory associated with the png file
 */
void io::PngWriter::free()
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