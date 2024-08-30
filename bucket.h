#ifndef BUCKET_H
#define BUCKET_H

#include "common.h"

#define ALLOCS_PER_BUCKET 256

typedef struct {
	uint32_t chunk_size_;
	bool is_free_;
	void* bucketHeader;
} ChunkHeader, *pChunkHeader;

typedef struct {
	void* flink;
	void* blink;
} LINK_LIST, *PLINK_LIST;

typedef struct {
	size_t bucket_size_;
	void* allocations_[ALLOCS_PER_BUCKET];
	uint32_t busy_;
	uint32_t free_;
	LINK_LIST moreBuckets;
} BucketHeader, *pBucketHeader;

#endif
