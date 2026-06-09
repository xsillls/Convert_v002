#include "System.h"



Convert& Convert::Get()
{
	static Convert object;
	return object;
}