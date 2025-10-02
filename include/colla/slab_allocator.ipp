#pragma once

#include "slab_allocator.hpp"
#include <cmath>
#include <cstddef>
#include <limits>
#include <new>
#include <sys/mman.h>
#include <unistd.h>
#include <utility>

namespace colla {

template <typename T>
SlabAllocator<T>::SlabAllocator(std::size_t min_cache_size)
    : min_cache_size(min_cache_size), map_size(0), cache_size(0) {
    std::size_t page_size = getpagesize();
    std::size_t pages_needed = static_cast<std::size_t>(
        std::ceil(static_cast<double>(sizeof(SlabCache<T>) +
                                      sizeof(Slab<T>) * min_cache_size) /
                  static_cast<double>(page_size)));
    map_size = pages_needed * page_size;
    cache_size = (map_size - sizeof(SlabCache<T>)) / sizeof(Slab<T>);
    allocate_cache();
}

template <typename T>
SlabAllocator<T>::SlabAllocator(const SlabAllocator& other)
    : min_cache_size(other.min_cache_size), map_size(other.map_size),
      cache_size(other.cache_size) {
    allocate_cache();
}

template <typename T>
SlabAllocator<T>::SlabAllocator(SlabAllocator&& other) noexcept
    : min_cache_size(std::exchange(other.min_cache_size, 0)),
      map_size(std::exchange(other.map_size, 0)),
      cache_size(std::exchange(other.cache_size, 0)),
      cache_list(std::exchange(other.cache_list, nullptr)),
      allocated_slabs(std::exchange(other.allocated_slabs, 0)) {}

template <typename T> SlabAllocator<T>::~SlabAllocator() {
    for (SlabCache<T>* cur_cache = cache_list; cur_cache;) {
        SlabCache<T>* next_cache = cur_cache->next;
        munmap(cur_cache, map_size);
        cur_cache = next_cache;
    }
}

template <typename T>
SlabAllocator<T>& SlabAllocator<T>::operator=(const SlabAllocator& other) {
    SlabAllocator alloc(other);
    *this = alloc;
    return *this;
}

template <typename T>
[[nodiscard]] T* SlabAllocator<T>::allocate(std::size_t n) {
    if (n != 1) {
        throw std::bad_alloc();
    }
    return allocate();
}

template <typename T> [[nodiscard]] T* SlabAllocator<T>::allocate() {
    std::size_t max_used = std::numeric_limits<std::size_t>::min();
    SlabCache<T>* best_cache = nullptr;
    for (SlabCache<T>* cur_cache = cache_list; cur_cache;
         cur_cache = cur_cache->next) {
        if (cur_cache->used >= cur_cache->size || cur_cache->used < max_used)
            continue;
        max_used = cur_cache->used;
        best_cache = cur_cache;
    }
    if (!best_cache) {
        allocate_cache();
        best_cache = cache_list;
    }

    Slab<T>* slab = best_cache->free_list;
    best_cache->free_list = slab->next;
    best_cache->used++;
    slab->next = nullptr;
    return &slab->data;
}

template <typename T>
void SlabAllocator<T>::deallocate(T* p, std::size_t) noexcept {
    deallocate(p);
}

template <typename T> void SlabAllocator<T>::deallocate(T* p) noexcept {
    SlabCache<T>* prev_cache = nullptr;
    SlabCache<T>* cache = nullptr;
    for (SlabCache<T>* cur_cache = cache_list; cur_cache;
         cur_cache = cur_cache->next) {
        if ((reinterpret_cast<std::size_t>(p) -
             reinterpret_cast<std::size_t>(cur_cache) - sizeof(SlabCache<T>)) %
                sizeof(Slab<T>) ==
            0) {
            cache = cur_cache;
            break;
        }
        prev_cache = cur_cache;
    }

    if (!cache)
        return;

    Slab<T>* slab = reinterpret_cast<Slab<T>*>(
        reinterpret_cast<std::size_t>(p) - sizeof(Slab<T>*));
    slab->next = cache->free_list;
    cache->free_list = slab;
    cache->used--;

    if (!cache->used) {
        if (prev_cache) {
            prev_cache->next = cache->next;
        } else {
            cache_list = cache->next;
        }
        munmap(cache, map_size);
    }
}

template <typename T> void SlabAllocator<T>::allocate_cache() {
    SlabCache<T>* new_cache = static_cast<SlabCache<T>*>(
        mmap(nullptr, map_size, PROT_READ | PROT_WRITE,
             MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
    ::new (new_cache) SlabCache<T>(cache_size, cache_list);
    cache_list = new_cache;
}

template <typename T>
SlabCache<T>::SlabCache(std::size_t size, SlabCache* next)
    : next(next), free_list(nullptr), size(size) {
    for (std::size_t i = 0; i < size; ++i) {
        Slab<T>* new_slab = reinterpret_cast<Slab<T>*>(
            reinterpret_cast<std::size_t>(this) + sizeof(SlabCache<T>) +
            i * sizeof(Slab<T>));
        new_slab->next = free_list;
        free_list = new_slab;
    }
}

} // namespace colla
