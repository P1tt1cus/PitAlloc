#ifndef PITALLOC_H
#define PITALLOC_H
#include "bucket.h"

#define ROUNDED_SIZE 16
#define FREE_HOLD_THRESHOLD 20
#define MAXIMUM_BUCKETS 256
#define SHIM_EXPORT __declspec(noinline)

typedef struct {
	BucketHeader bucket_headers[MAXIMUM_BUCKETS];
} MemoryPitt, *pMemoryPitt;

class PitAlloc
{
  public:
    PitAlloc();
    ~PitAlloc();
    void* alloc(size_t size);
    void free(void* ptr);
		static PitAlloc& getInstance();
  private:
		pBucketHeader getBucketHeader(size_t size);
		pBucketHeader newBucket(pBucketHeader bucketHeader);
		size_t roundSize(size_t size);
		pMemoryPitt memoryPitt_;
};

#endif
