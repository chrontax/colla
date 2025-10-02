#pragma once

#include <cstddef>

namespace colla {

/**
 * A simple arena allocator that allocates memory in contiguous blocks.
 * The memory is only freed upon destruction or when reset() is called.
 *
 * @tparam T The type of elements to allocate.
 */
template <typename T> struct ArenaAllocator {
    using value_type = T;

    /**
     * Constructs an arena that can hold at least \p min_capacity elements.
     * It may fit more to fill out the allocated memory pages.
     */
    ArenaAllocator(std::size_t min_capacity);
    ArenaAllocator(const ArenaAllocator&);
    ArenaAllocator(ArenaAllocator&&) noexcept;
    ~ArenaAllocator();

    ArenaAllocator& operator=(const ArenaAllocator&);

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other)
        : ArenaAllocator(other.min_capacity()) {}

    template <typename U> struct rebind {
        using other = ArenaAllocator<U>;
    };

    /**
     * Throws std::bad_alloc if there is not enough memory in the arena.
     */
    [[nodiscard]] T* allocate(std::size_t n);
    constexpr void deallocate(T*, std::size_t) noexcept {}

    /**
     * Resets the arena, making all previously allocated memory available again.
     * Does not free any memory back to the OS.
     */
    void reset();
    constexpr std::size_t capacity() const noexcept;
    constexpr std::size_t used() const noexcept;
    constexpr std::size_t min_capacity() const noexcept;

  private:
    std::size_t _capacity = 0;
    std::size_t _min_capacity;
    std::size_t _used = 0;
    std::size_t arena_size = 0;
    T* base = nullptr;
};

} // namespace colla

#include "arena_allocator.ipp" // IWYU pragma: export
