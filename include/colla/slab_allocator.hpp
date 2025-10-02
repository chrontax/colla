#pragma once

#include <cstddef>

namespace colla {

template <typename T> class SlabCache;
template <typename T> class Slab;

/**
 * A simple slab allocator that allocates memory in fixed-size chunks (slabs).
 * Each allocation is of size 1, and deallocations can be done in any order.
 * Memory is returned to the OS when a cache becomes completely free.
 *
 * @tparam T The type of elements to allocate.
 */
template <typename T> struct SlabAllocator {
    using value_type = T;

    /**
     * Constructs a slab allocator with a cache size of at least \p
     * min_cache_size. The cache size may be larger to fill out allocated memory
     * pages.
     */
    SlabAllocator(std::size_t min_cache_size);
    SlabAllocator(const SlabAllocator&);
    SlabAllocator(SlabAllocator&&) noexcept;
    ~SlabAllocator();

    SlabAllocator& operator=(const SlabAllocator&);

    template <typename U>
    SlabAllocator(const SlabAllocator<U>& other)
        : SlabAllocator(other.min_cache_size) {}

    template <typename U> struct rebind {
        using other = SlabAllocator<U>;
    };

    /**
     * Throws std::bad_alloc if n != 1.
     */
    [[nodiscard]] T* allocate(std::size_t n);
    [[nodiscard]] T* allocate();
    void deallocate(T* p, std::size_t) noexcept;
    void deallocate(T* p) noexcept;

  private:
    std::size_t min_cache_size;
    std::size_t map_size;
    std::size_t cache_size;
    SlabCache<T>* cache_list = nullptr;
    std::size_t allocated_slabs = 0;

    void allocate_cache();

    template <typename U> friend struct SlabAllocator;
};

template <typename T> class SlabCache {
    SlabCache* next;
    Slab<T>* free_list;
    std::size_t size;
    std::size_t used = 0;

    SlabCache(std::size_t size, SlabCache* next = nullptr);

    friend struct SlabAllocator<T>;
};

template <typename T> class Slab {
    Slab* next = nullptr;
    T data;

    friend class SlabCache<T>;
    friend struct SlabAllocator<T>;
};

} // namespace colla

#include "slab_allocator.ipp" // IWYU pragma: export
