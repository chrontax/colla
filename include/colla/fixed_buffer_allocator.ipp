#pragma once

#include "fixed_buffer_allocator.hpp"
#include <cstddef>
#include <new>

namespace colla {

template <typename T, std::size_t N>
[[nodiscard]] constexpr T* FixedBufferAllocator<T, N>::allocate(std::size_t n) {
    if (_used + n > N) {
        throw std::bad_alloc();
    }

    T* ptr = &storage[_used];
    _used += n;
    return ptr;
}

template <typename T, std::size_t N>
constexpr void FixedBufferAllocator<T, N>::reset() noexcept {
    _used = 0;
}

} // namespace colla
