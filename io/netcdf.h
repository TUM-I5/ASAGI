#ifndef IONETCDF_H
#define IONETCDF_H

#include <ncFile.h>

namespace io
{
	class NetCdf
	{
	private:
		const char* m_filename;
		/** the file, this class will work on */
		const netCDF::NcFile* m_file;
		bool m_open;
		/** this is true, if we need to switch x and y wenn accessing data */
		bool m_dimSwitched;
		
	public:
		NetCdf(const char* filename);
		virtual ~NetCdf();
		bool open();
		bool isOpen() const;
		unsigned long getXDim();
		unsigned long getYDim();
		float getXOffset();
		float getYOffset();
		float getXScaling();
		float getYScaling();
		float getMin();
		float getMax();
		float* getAll();
		float getDefault();
	};
};

#endif