# Colla

A header-only memory allocator library written in C++, compatible with STL containers.

## Features

- No dependencies, header-only.
- Compatible with STL containers.
- Multiple allocators:
  - `colla::FixedBufferAllocator<T>`: Allocates memory from a buffer of fixed size.
  - `colla::ArenaAllocator<T>`: Allocates memory from a pre-allocated arena.
  - `colla::SlabAllocator<T>`:
    Allocates memory in slabs from a cache, allocating another cache if all are full.

## Usage

You can look into `examples/` for example usage of each allocator.
You can build them by turning on the `BUILD_EXAMPLES` option in CMake.

## Compatibility

Due to being backed by mmap, slab and arena allocators work only on POSIX systems.

## License

This project is licensed under the MIT License.
See the [LICENSE](LICENSE) file for details.
