#pragma once

#include <array>
#include <cstddef>

namespace colla {

/**
 * A simple fixed-size buffer allocator that allocates memory from a
 * pre-allocated buffer. The memory is only freed when reset() is called.
 *
 * @tparam T The type of elements to allocate.
 * @tparam N The number of elements in the fixed buffer.
 */
template <typename T, std::size_t N> struct FixedBufferAllocator {
    using value_type = T;

    constexpr FixedBufferAllocator() = default;

    template <typename U>
    constexpr FixedBufferAllocator(const FixedBufferAllocator<U, N>&) noexcept {
    }

    template <typename U> struct rebind {
        using other = FixedBufferAllocator<U, N>;
    };

    /**
     * Throws std::bad_alloc if the buffer is exhausted.
     */
    [[nodiscard]] constexpr T* allocate(std::size_t n);
    constexpr void deallocate(T*, std::size_t) noexcept {}

    /**
     * Resets the allocator, making all previously allocated memory available.
     */
    constexpr void reset() noexcept;

  private:
    std::array<T, N> storage = {};
    std::size_t _used = 0;
};

} // namespace colla

#include "fixed_buffer_allocator.ipp" // IWYU pragma: export
