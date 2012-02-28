#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <asagi.h>

#include <cassert>
#include <netcdf>
#include <debug/dbg.h>

namespace io
{
	class NetCdf
	{
	private:
		const char* m_filename;
		
		/** MPI rank */
		const int m_rank;
		
		/** the file, this class will work on */
		const netCDF::NcFile* m_file;
		
		bool m_open;
		
		/** The variable we read */
		netCDF::NcVar m_variable;
		
		/** Number of dimension (1, 2 or 3) */
		int m_dimensions;
		
		std::string m_nameX;
		std::string m_nameY;
		std::string m_nameZ;
	public:
		NetCdf(const char* filename, int rank);
		virtual ~NetCdf();
		asagi::Grid::Error open(const char* varname = "z");
		bool isOpen() const;
		unsigned long getXDim();
		unsigned long getYDim();
		unsigned long getZDim();
		double getXOffset();
		double getYOffset();
		double getZOffset();
		double getXScaling();
		double getYScaling();
		double getZScaling();
		
		template<typename T> void getVar(T *var,
			size_t xoffset = 0, size_t yoffset = 0, size_t zoffset = 0,
			size_t xsize = 0, size_t ysize = 0, size_t zsize = 0)
		{
			size_t y;
			std::vector<size_t> start(m_dimensions);
			std::vector<size_t> count(m_dimensions);
			
			if (xsize == 0)
				xsize = getXDim();
			
			if (ysize == 0)
				ysize = getYDim();
			
			if (zsize == 0)
				zsize = getZDim();
			
			switch (m_dimensions) {
			case 1:
				if (xoffset + xsize > getXDim())
					xsize = getXDim() - xoffset;
				
				start[0] = xoffset;
				
				count[0] = xsize;
				break;
			case 2:
				if (yoffset + ysize > getYDim())
					ysize = getYDim() - yoffset;
			
				start[0] = yoffset;
				start[1] = xoffset;
			
				count[0] = ysize;
				count[1] = xsize;
			
				if (xoffset + xsize > getXDim()) {
					// we have to load data row by row
				
					count[0] = 1;
					count[1] = getXDim() - xoffset;
				
					for (size_t i = 0; i < ysize; i++) {
						m_variable.getVar(start, count,
							static_cast<T*>(&var[i * xsize]));
					
						// Add 1 to the yoffset
						start[0]++;
					}
				
					return;
				}
				
				break;
			case 3:
				if (zoffset + zsize > getZDim())
					zsize = getZDim() - zoffset;
				
				start[0] = zoffset;
				start[1] = yoffset;
				start[2] = xoffset;
				
				count[0] = zsize;
				count[1] = ysize;
				count[2] = xsize;
				
				if (xoffset + xsize > getXDim()) {
					// we have to load data row by row
					
					count[0] = 1;
					count[1] = 1;
					count[2] = getXDim() - xoffset;
					
					y = ysize;
					if (yoffset + ysize > getYDim())
						y = getYDim() - yoffset;
					
					for (size_t i = 0; i < zsize; i++) {
						// loop through z dimension
						for (size_t j = 0; j < y; j++) {
							// loop through y dimension
							m_variable.getVar(start, count,
								static_cast<T*>(&var[(i * ysize + j) * xsize]));
							
							// Add 1 to yoffset
							start[1]++;
						}
						
						// Add 1 to zoffset
						start[0]++;
						start[1] = yoffset;
					}
					return;
				}
				
				if (yoffset + ysize > getYDim()) {
					// we can do this slice by slice at least
					
					count[0] = 1;
					count[1] = getYDim() - yoffset;
					
					for (size_t i = 0; i < zsize; i++) {
						m_variable.getVar(start, count,
							static_cast<T*>(&var[i * ysize * xsize]));
						
						// Add 1 to zoffset
						start[0]++;
					}
					
					return;
				}
				
				break;
			default:
				assert(false);
			}
			
			m_variable.getVar(start, count, var);
		}
		
		unsigned int getVarSize();
	};
}

#endif // IO_NETCDF_H
