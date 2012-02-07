#ifndef IO_NETCDF_H
#define IO_NETCDF_H

#include <ncAtt.h>
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
		double getXOffset();
		double getYOffset();
		double getXScaling();
		double getYScaling();
		
		template<typename T> void getVar(T *var,
			size_t xoffset = 0, size_t yoffset = 0,
			size_t xsize = 0, size_t ysize = 0)
		{
			// TODO test m_dimSwitched = 1!!!
			
			netCDF::NcVar z = m_file->getVar("z");
			std::vector<size_t> start(2);
			std::vector<size_t> count(2);
			std::vector<ptrdiff_t> stride(2, 1);
			std::vector<ptrdiff_t> imap(2);
			
			if (xsize == 0)
				xsize = getXDim();
			
			if (ysize == 0)
				ysize = getYDim();
			
			if (yoffset + ysize > getYDim())
				ysize = getYDim() - yoffset;
			
			if (m_dimSwitched) {
				start[0] = xoffset;
				start[1] = yoffset;
				
				count[0] = xsize;
				count[1] = ysize;
				
				imap[0] = 1;
				imap[1] = count[0];
			} else {
				start[0] = yoffset;
				start[1] = xoffset;
				
				count[0] = ysize;
				count[1] = xsize;
				
				imap[0] = count[1];
				imap[1] = 1;
			}
			
			if (xoffset + xsize > getXDim()) {
				// This is difficult, we have to load
				// data row by row
				
				if (m_dimSwitched) {
					count[0] = getXDim() - xoffset;
					count[1] = 1;
					
					//imap[0] = 1;	// already correct
					imap[1] = count[0];
				} else {
					count[0] = 1;
					count[1] = getXDim() - xoffset;
					
					imap[0] = count[1];
					//imap[1] = 1;	// already correct
				}
				
				for (size_t i = 0; i < ysize; i++) {
					z.getVar(start, count, stride, imap,
						static_cast<T*>(&var[i * xsize]));
					
					// Add 1 to the yoffset
					start[(m_dimSwitched ? 1 : 0)]++;
				}
				
				return;
			}
			
			z.getVar(start, count, stride, imap, var);
		}
		
		unsigned int getVarSize();
		
		/**
		 * Reads the "missing_value" attribute from the netcdf file.
		 * If this attribute is missing, defaultValue is set to 0.
		 * 
		 * After COARDS conventions, the attribute should have the same
		 * type as the variable. (See:
		 * <a>http://ferret.wrc.noaa.gov/noaa_coop/coop_cdf_profile.html</a>)
		 * 
		 * @deprecated No longer called, since we don't support default
		 *  values anymore
		 */
		template<typename T> void getDefault(T *defaultValue)
		{
			netCDF::NcVarAtt att;
			
			try {
				att = m_file->getVar("z")
					.getAtt("missing_value");
			} catch (netCDF::exceptions::NcException& e) {
				// Attribute missing
				*defaultValue = 0;
				return;
			}
			
			att.getValues(defaultValue);
		}
	};

	template<> void NetCdf::getVar<void>(void* var,
			size_t xoffset, size_t yoffset,
			size_t xsize, size_t ysize);
	template<> void NetCdf::getDefault<void>(void* defaultValue);
};

#endif // IO_NETCDF_H