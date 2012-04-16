#ifndef SIMPLEGRIDCONTAINER_H
#define SIMPLEGRIDCONTAINER_H

#include "gridcontainer.h"

class SimpleGridContainer : public GridContainer
{
private:
	/** All grids we control */
	::Grid **m_grids;
public:
	SimpleGridContainer(Type type, bool isArray = false,
		unsigned int hint = asagi::NO_HINT,
		unsigned int levels = 1);
	virtual ~SimpleGridContainer();
	
	Error setParam(const char* name, const char* value,
		unsigned int level = 0);
	Error open(const char* filename, unsigned int level = 0);
	
	char getByte3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	int getInt3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	long getLong3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	float getFloat3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	double getDouble3D(double x, double y = 0, double z = 0,
		unsigned int level = 0);
	void getBuf3D(void* buf, double x, double y = 0, double z = 0,
		unsigned int level = 0);
	
	bool exportPng(const char* filename, unsigned int level = 0);
};

#endif // SIMPLEGRIDCONTAINER_H
