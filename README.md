# PitAlloc

PitAlloc is a basic memory allocator written in C++ that uses a bucket system. It is inspired by partition alloc and is currently a Proof of Concept (PoC).

## Features

- Bucket-based memory allocation
- Singleton design pattern for allocator instance
- Basic error handling
- Allocation and deallocation (free) functions
- Supports limited allocation sizes

## Getting Started

### Prerequisites

- C++ compiler (e.g., g++, clang++)

### Building

To compile the project, use the following commands in your terminal:

```sh
git clone https://github.com/P1tt1cus/PitAlloc.git
cd PitAlloc
g++ -o pitalloc pitalloc.cc -std=c++11
```

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
Inspired by partition alloc.
