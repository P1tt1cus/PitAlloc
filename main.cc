#include "shim_cpp_exports.h"
#include "logger.h"

class Test
{
	public:
		Test(int param) : test_(param) {};
		int test_;
};

int main() 
{
	void* ptrs[256*3];

	LOG(Level::info) << "Allocating Ptrs";
	LOG(Level::info) << "================";

	for (size_t i = 0; i < 256*3; i++)
	{
		Test* newPtr = new Test(0x10);
		LOG(Level::info) << "new ptr: " << newPtr;
 		ptrs[i] = newPtr;
	}

	// Multiple buckets should exist within the linked list 
	// as part of the 16-byte bucket. 
	// __debugbreak();
	LOG(Level::info) << "Free all the Ptrs";
	LOG(Level::info) << "=================";

	for (size_t i = 0; i < 256*3; i++)
	{
		LOG(Level::info) << "delete ptr: " << ptrs[i];
		delete ptrs[i];
	}

	// Each bucket in the 16-byte linked list bar the first
	// one should be empty but still have reserved free ptrs. 
	// __debugbreak();

	LOG(Level::info) << "Allocating Ptrs - AGAIN";
	LOG(Level::info) << "=======================";

	// This should use some of the reserved free ptrs in the first
	// available bucket, once it is exhausted, it will begin requesting
	// memory from the OS. 
	for (size_t i = 0; i < 256; i++)
	{
		Test* newPtr = new Test(0x10);
		LOG(Level::info) << "new ptr: " << newPtr;
		ptrs[i] = newPtr;
	}

	LOG(Level::info) << "Finished";
	
  return 0;
}
