#include <colla/fixed_buffer_allocator.hpp>
#include <iostream>
#include <new>
#include <vector>

int main() {
    std::vector<int, colla::FixedBufferAllocator<int, 10>> vec;
    vec.reserve(10);

    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    std::cout << "Vector contents: ";
    for (auto n : vec) {
        std::cout << n << ' ';
    }
    std::cout << '\n';

    try {
        vec.push_back(10);
    } catch (const std::bad_alloc& e) {
        std::cout << "Oh no, we're out of space! " << e.what() << std::endl;
    }

    return 0;
}
