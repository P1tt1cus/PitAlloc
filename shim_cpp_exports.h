#ifndef SHIM_CPP_EXPORTS_H
#define SHIM_CPP_EXPORTS_H

#include "common.h"
#include "pitalloc.h"

SHIM_EXPORT void* operator new[](size_t size);
SHIM_EXPORT void* operator new(size_t size);
SHIM_EXPORT void operator delete[](void* ptr);
SHIM_EXPORT void operator delete(void* ptr);

#endif
