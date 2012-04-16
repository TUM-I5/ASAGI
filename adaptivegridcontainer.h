#ifndef ADAPTIVEGRIDCONTAINER_H
#define ADAPTIVEGRIDCONTAINER_H

#include "gridcontainer.h"

class Grid;

class AdaptiveGridContainer : public GridContainer
{
private:
	const int m_hint;
	
	/** Grid(-parts) */
	std::vector< ::Grid*>* m_grids;
	
	/** Next unique id for a grid */
	unsigned int m_ids;
	
#ifdef THREADSAFETY
	/** Lock the open function ... */
	std::mutex m_mutex;
#endif // THREADSAFETY
	
public:
	AdaptiveGridContainer(Type type, bool isArray = false,
		unsigned int hint = asagi::NO_HINT,
		unsigned int level = 1);
	virtual ~AdaptiveGridContainer();
	
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
		
private:
	::Grid* getGrid(double x, double y = 0, double z = 0,
		unsigned int level = 0);
};

#endif // ADAPTIVEGRIDCONTAINER_H
