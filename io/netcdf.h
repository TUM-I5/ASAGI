#ifndef IONETCDF_H
#define IONETCDF_H

#include <ncFile.h>
#include <ncVar.h>

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
		
		template<typename T> void getVar(T *var)
		{
			netCDF::NcVar z;
			std::vector<size_t> start(2, 0);
			std::vector<size_t> count(2);
			std::vector<ptrdiff_t> stride(2, 1);
			std::vector<ptrdiff_t> imap(2);
			
			if (m_dimSwitched) {
				count[0] = getXDim();
				count[1] = getYDim();
				
				imap[0] = 1;
				imap[1] = count[0];
			} else {
				count[0] = getYDim();
				count[1] = getXDim();
				
				imap[0] = count[1];
				imap[1] = 1;
			}
			
			z = m_file->getVar("z");
			z.getVar(start, count, stride, imap, var);
		}
		
		template<typename T> void getDefault(T &defaultValue)
		{
			// TODO
			defaultValue = 0;
		}
	};
};

#endif