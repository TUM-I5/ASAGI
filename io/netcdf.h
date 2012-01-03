#ifndef IONETCDF_H
#define IONETCDF_H

#include <netcdfcpp.h>

namespace io
{
	class NetCdf
	{
	private:
		/** the file, this class will work on */
		const NcFile file;
		bool error;
		/** this is true, if we need to switch x and y wenn accessing data */
		bool dimSwitched;
		
	public:
		NetCdf(const char* fileName);
		bool hasError();
		unsigned long getXDim();
		unsigned long getYDim();
		bool isDimSwitched();
		float* getAll();
	};
};

#endif