#pragma once

#include <array>
#include <cstddef>

namespace colla {

template <typename T, std::size_t N> struct FixedBufferAllocator {
    using value_type = T;

    constexpr FixedBufferAllocator() = default;

    template <typename U>
    constexpr FixedBufferAllocator(const FixedBufferAllocator<U, N>&) noexcept {
    }

    template <typename U> struct rebind {
        using other = FixedBufferAllocator<U, N>;
    };

    [[nodiscard]] constexpr T* allocate(std::size_t n);
    constexpr void deallocate(T*, std::size_t) noexcept {}

    constexpr void reset() noexcept;

  private:
    std::array<T, N> storage = {};
    std::size_t _used = 0;
};

} // namespace colla

#include "fixed_buffer_allocator.ipp" // IWYU pragma: export
