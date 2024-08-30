#include "shim_cpp_exports.h"

PitAlloc* g_palloc = nullptr;

#define __THROW

SHIM_EXPORT void* operator new[](size_t size) {
	if (g_palloc == nullptr) 
	{
		g_palloc = &PitAlloc::getInstance();
	}
	void* ptr = g_palloc->alloc(size);
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
  return ptr;
}


SHIM_EXPORT void* operator new(size_t size) {
 	if (g_palloc == nullptr) 
	{
		g_palloc = &PitAlloc::getInstance();
	}
	void* ptr = g_palloc->alloc(size);
  if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
  return ptr;
}


SHIM_EXPORT void operator delete[](void* ptr) __THROW {
	if (g_palloc == nullptr) 
	{
		g_palloc = &PitAlloc::getInstance();
	}
  g_palloc->free(ptr);
}


SHIM_EXPORT void operator delete(void* ptr) {
 	if (g_palloc == nullptr) 
	{
		g_palloc = &PitAlloc::getInstance();
	}
  g_palloc->free(ptr);
}
