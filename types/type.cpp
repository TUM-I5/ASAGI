#include "type.h"
#include <cstring>

/**
 * Check compatibility of the input file with this type.
 */
bool types::Type::check(io::NetCdf& file)
{
	// Default: everything is okay
	return true;
}

/**
 * This is the "no-conversation" function. It simple copys a whole variable
 * from data to buf
 */
void types::Type::convertBuffer(void* data, void* buf)
{
	memcpy(buf, data, getSize());
}
