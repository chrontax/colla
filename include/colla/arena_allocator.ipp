#pragma once

#include "arena_allocator.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <new>
#include <utility>

#include <sys/mman.h>
#include <unistd.h>

namespace colla {

template <typename T>
ArenaAllocator<T>::ArenaAllocator(std::size_t min_capacity)
    : _min_capacity(min_capacity) {
    if (!min_capacity)
        return;

    int page_size = getpagesize();
    std::size_t pages_needed = static_cast<std::size_t>(
        std::ceil(static_cast<double>(min_capacity * sizeof(T)) /
                  static_cast<double>(page_size)));
    arena_size = pages_needed * page_size;
    _capacity = arena_size / sizeof(T);
    base = static_cast<T*>(mmap(nullptr, arena_size, PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}

template <typename T>
ArenaAllocator<T>::ArenaAllocator(const ArenaAllocator& other)
    : _capacity(other._capacity), _min_capacity(other._min_capacity),
      _used(other._used), arena_size(other.arena_size) {
    if (arena_size) {
        base = static_cast<T*>(mmap(nullptr, arena_size, PROT_READ | PROT_WRITE,
                                    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
        std::copy(other.base, &other.base[_used], base);
    }
}

template <typename T>
ArenaAllocator<T>& ArenaAllocator<T>::operator=(const ArenaAllocator& other) {
    ArenaAllocator alloc(other);
    this = alloc;
    return *this;
}

template <typename T>
ArenaAllocator<T>::ArenaAllocator(ArenaAllocator&& other) noexcept
    : _capacity(std::exchange(other._capacity, 0)),
      _min_capacity(std::exchange(other._min_capacity, 0)),
      _used(std::exchange(other._used, 0)),
      arena_size(std::exchange(other.arena_size, 0)),
      base(std::exchange(other.base, nullptr)) {}

template <typename T> ArenaAllocator<T>::~ArenaAllocator() {
    munmap(base, arena_size);
}

template <typename T>
[[nodiscard]] T* ArenaAllocator<T>::allocate(std::size_t n) {
    if (_used + n > _capacity) {
        throw std::bad_alloc();
    }
    T* ptr = &base[_used];
    _used += n;
    return ptr;
}

template <typename T> void ArenaAllocator<T>::reset() {
    _used = 0;
    base = static_cast<T*>(mmap(nullptr, arena_size, PROT_READ | PROT_WRITE,
                                MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
}

template <typename T>
constexpr std::size_t ArenaAllocator<T>::capacity() const noexcept {
    return _capacity;
}

template <typename T>
constexpr std::size_t ArenaAllocator<T>::used() const noexcept {
    return _used;
}

template <typename T>
constexpr std::size_t ArenaAllocator<T>::min_capacity() const noexcept {
    return _min_capacity;
}

} // namespace colla
