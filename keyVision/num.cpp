
#include "num.h"
_num& num()
{
	static _num _inst;
	return _inst;
}