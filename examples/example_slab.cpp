#include <colla/slab_allocator.hpp>
#include <iostream>
#include <list>
#include <new>
#include <vector>

using namespace colla;

int main() {
    std::cout << "Creating a slab alloc for at least 10 longs\n";
    SlabAllocator<long> alloc(10);

    std::list<long, SlabAllocator<long>> list(alloc);

    std::cout << "Adding numbers to the list...\n";

    // ~512 fit in one page, so this should allocate another cache
    for (long i = 0; i < 600; ++i) {
        list.push_back(i);
        std::cout << '\r' << i;
    }

    std::cout << "\nDeleting even numbers from the list...\n";
    for (auto it = list.begin(); it != list.end();) {
        if (*it % 2 == 0) {
            std::cout << '\r' << *it;
            it = list.erase(it);
        } else {
            ++it;
        }
    }

    try {
        std::vector<long, SlabAllocator<long>> vec(10, alloc);
    } catch (const std::bad_alloc& e) {
        // Expected, because SlabAllocator only supports allocations of size 1
        std::cout << "\nCouldn't allocate vector: " << e.what() << '\n';
    }

    return 0;
}
