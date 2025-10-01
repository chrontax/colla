#pragma once

#include <cstddef>

namespace colla {

template <typename T> struct ArenaAllocator {
    using value_type = T;

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

    [[nodiscard]] T* allocate(std::size_t n);
    constexpr void deallocate(T*, std::size_t) noexcept {}
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
