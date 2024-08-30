#include "pitalloc.h"
#include "bucket.h"

PitAlloc::PitAlloc() 
{
	memoryPitt_ = reinterpret_cast<pMemoryPitt>(std::malloc(sizeof(MemoryPitt)));
	if (memoryPitt_ == nullptr)
	{
		throw std::bad_alloc();
	}
	std::memset(memoryPitt_, 0, sizeof(MemoryPitt));
}

PitAlloc::~PitAlloc() 
{
	std::free(memoryPitt_);
}

void* PitAlloc::alloc(size_t size)
{
	// Round the size of the allocation 
	size_t roundedSize = roundSize(size);
	
	// Retrieve the bucket for the allocations - 
	// this will ensure a non-full bucket is returned
	BucketHeader* bucketHeader = getBucketHeader(roundedSize); 

	// The bucket is full, we have not handled this logic yet
	// TODO: Over 256 requires a new bucket.
	size_t allocIndex = bucketHeader->busy_;
	if (allocIndex > ALLOCS_PER_BUCKET)
	{
		// Create a new bucket in a 
		// linked doubly linked list and do shit
		return nullptr;
	
	}

	// Find the first free allocation in the bucket
	// and use it.
	for (size_t i = 0; i < ALLOCS_PER_BUCKET; i++) 
	{
		// If the allocation slot has a nullptr it is free to be used
		// for a *NEW* allocation.
		if (bucketHeader->allocations_[i] == nullptr)
		{
			// Create the allocation with the chunk header
			pChunkHeader chunkHeader = reinterpret_cast<pChunkHeader>(
					std::malloc(bucketHeader->bucket_size_ + sizeof(ChunkHeader)));	
			std::memset(chunkHeader, 0, bucketHeader->bucket_size_ + sizeof(ChunkHeader));
			chunkHeader->chunk_size_ = bucketHeader->bucket_size_;
			chunkHeader->is_free_ = false;
			chunkHeader->bucketHeader = bucketHeader;
			
			// Place our allocation into the first available
			// free entry for our bucket.
			bucketHeader->allocations_[i] =
				reinterpret_cast<char*>(chunkHeader) + sizeof(ChunkHeader);
			
			// Increment total number of busy allocations 
			bucketHeader->busy_++;
			return bucketHeader->allocations_[i];

		}
		
		// If the allocation slot has a ptr, *AND* it is marked as free'd
		// we can recycle the allocation for hopeful performance gains.
		else if (bucketHeader->allocations_[i] != nullptr)
		{
			pChunkHeader chunkHeader = reinterpret_cast<pChunkHeader>( 
				reinterpret_cast<char*>(bucketHeader->allocations_[i]) - sizeof(ChunkHeader));
			if (chunkHeader->is_free_)
			{
				chunkHeader->is_free_ = false;
				bucketHeader->busy_++;
				bucketHeader->free_--;
				return bucketHeader->allocations_[i];
			}
		}
	}

	// Unable to find free allocation in bucket, something is wrong.
	return nullptr;

}

void PitAlloc::free(void* ptr)
{
	// Retrieve the ChunkHeader and set the is_free to true
	pChunkHeader chunkHeader = reinterpret_cast<pChunkHeader>(
		reinterpret_cast<char*>(ptr) - sizeof(ChunkHeader));
	chunkHeader->is_free_ = true;

	// Always zero the data of a free chunk
	std::memset(ptr, 0, chunkHeader->chunk_size_);

	// Retrieve the bucket for the allocations 
	pBucketHeader bucketHeader = reinterpret_cast<pBucketHeader>(
			chunkHeader->bucketHeader);

	// If free 'holding' threshold has been reached for the bucket 
	// release it back to the operating system.
	if (bucketHeader->free_ >= FREE_HOLD_THRESHOLD)
	{
		for (size_t i = 0; i < ALLOCS_PER_BUCKET; i++)
		{
			// Find the Ptr inside the bucket to set it to nullptr
			if (bucketHeader->allocations_[i] == ptr)
			{
				bucketHeader->allocations_[i] = nullptr;
				std::free(chunkHeader);
				break;
			}
		}
	} 
	// hold onto the memory and just set the ptr to free'd
	else
	{
		bucketHeader->free_++;
	}
	// always decrement the number of busy allocations in a bucket
	// when it is free'd.
	bucketHeader->busy_--;
}


PitAlloc& PitAlloc::getInstance()
{
	static PitAlloc pitalloc_;
	return pitalloc_;
}


pBucketHeader PitAlloc::getBucketHeader(size_t size)
{
	pBucketHeader bucketHeader = nullptr;

	// Calculate the bucket index
	size_t bucketIndex = size / 16 - 1;

	// Ensure its not over the maximum bucket index
	// i.e. the allocation is too large... 
	// Currently PitAlloc only supports 256 * 16 as a MAX 
	// allocation size.
	if (bucketIndex > MAXIMUM_BUCKETS) return nullptr;

	// Retrieve the bucket for the allocations 
	bucketHeader = &memoryPitt_->bucket_headers[bucketIndex];
	
	// Check to see whether the bucket is full of allocations and 
	// needs a new bucket allocated within the doubly linked list
	if (bucketHeader->busy_ >= ALLOCS_PER_BUCKET && bucketHeader->free_ == 0)
	{
		// Iterate over the buckets until a bucket is found that is not full 
		// or has some free allocations... 
		while (bucketHeader->moreBuckets.flink != nullptr)
		{
			bucketHeader = reinterpret_cast<pBucketHeader>(
					bucketHeader->moreBuckets.flink);
			if (bucketHeader->busy_ < ALLOCS_PER_BUCKET && bucketHeader->free_ != 0)
			{
				break;
			}
		}
		// If we have reached a point where there are no additional 
		// buckets, we can create a new one.
		if (bucketHeader->moreBuckets.flink == nullptr &&
				bucketHeader->busy_ >= ALLOCS_PER_BUCKET &&
				bucketHeader->free_ == 0) 
		{
			bucketHeader = newBucket(bucketHeader);
		}
	}
	if (bucketHeader->bucket_size_ == 0)
	{
		bucketHeader->bucket_size_ = size;
	}
	return bucketHeader;
}

pBucketHeader PitAlloc::newBucket(pBucketHeader bucketHeader) {
    pBucketHeader newBucketHeader = reinterpret_cast<pBucketHeader>(
        std::malloc(sizeof(BucketHeader)));
    std::memset(newBucketHeader, 0, sizeof(BucketHeader));

    // If this is the first bucket header, initialize its blink to point to itself
    if (bucketHeader->moreBuckets.blink == nullptr) {
        bucketHeader->moreBuckets.blink = bucketHeader;
        bucketHeader->moreBuckets.flink = bucketHeader;
    }

    // Set the new bucket's links
    newBucketHeader->moreBuckets.blink = bucketHeader;
    newBucketHeader->moreBuckets.flink = bucketHeader->moreBuckets.flink;

    // Update the forward link of the current bucket's forward link
    if (bucketHeader->moreBuckets.flink != nullptr) {
        reinterpret_cast<pBucketHeader>(bucketHeader->moreBuckets.flink)->moreBuckets.blink = newBucketHeader;
    }

    // Update the current bucket's forward link
    bucketHeader->moreBuckets.flink = newBucketHeader;

    return newBucketHeader;
}

size_t PitAlloc::roundSize(size_t size)
{
	// Calculate the rounded size
	return ((size - 1) / ROUNDED_SIZE + 1) * ROUNDED_SIZE;
}
