/**
 * @file
 *  This file is part of ASAGI.
 * 
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 * 
 * @copyright 2012 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef IO_PNGWRITER_H
#define IO_PNGWRITER_H

#include <png.h>

namespace io
{

/**
 * C++ wrapper to generate PNG files
 */
class PngWriter
{
private:
	/** Width of the png */
	const unsigned long width;
	/** Height of the png */
	const unsigned long height;
	
	/** Filehandle we will use to write the png file */
	FILE* fp;
	/** Pointer to the png write struct */
	png_structp png_ptr;
	/** Pointer to the png info struct */
	png_infop info_ptr;
	
	/** Stores the pixels of the picture */
	png_byte** row_pointers;
public:
	PngWriter(unsigned long width, unsigned long height);
	virtual ~PngWriter()
	{
		free();
	}
	
	bool create(const char* filename);
	void write(unsigned long x, unsigned long y, unsigned char red,
		unsigned char gree, unsigned char blue);
	void close();
private:
	void free();
private:
	/** Number of bytes for each pixel; RGB -> 3 */
	static const char PIXEL_SIZE = 3;
};

}

#endif // IO_PNGWRITER_H