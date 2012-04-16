#ifndef NOMPIGRID_H
#define NOMPIGRID_H

#include "grid.h"

/**
 * This grid used when compiled without MPI support.
 * 
 * @todo This grid and {@link SimpleGrid} could share some functions.
 */
class NoMPIGrid : public Grid
{
private:
	unsigned char* m_data;

public:
	NoMPIGrid(GridContainer &container, unsigned int hint = asagi::NO_HINT);
	virtual ~NoMPIGrid();
	
protected:
	asagi::Grid::Error init();
	
	void getAt(void* buf, types::Type::converter_t converter,
		unsigned long x, unsigned long y = 0, unsigned long z = 0);
};

#endif // NOMPIGRID_H
