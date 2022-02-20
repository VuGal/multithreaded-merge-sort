#include "osapi.h"

namespace osapi {

unsigned int getSystemTime() {
	return GetTickCount();
}

} // namespace osapi
