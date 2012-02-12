#include "type.h"

/**
 * This is the "no-conversation" function. It simple copys a whole variable
 * from data to buf
 */
void types::Type::convertBuffer(void* data, void* buf)
{
	memcpy(buf, data, getSize());
}