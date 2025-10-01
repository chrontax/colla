#include <colla/arena_allocator.hpp>
#include <iostream>
#include <list>
#include <new>

using namespace colla;

int main() {
    std::cout << "Creating an arena for at least 10 longs\n";
    ArenaAllocator<long> alloc(10);

    std::list<long, ArenaAllocator<long>> list(alloc);

    std::cout << "Adding numbers to the list...\n";

    for (long i = 0;; ++i) {
        try {
            list.push_back(i);
            std::cout << '\r' << i;
        } catch (const std::bad_alloc& e) {
            std::cout << "\nWe've run out of space in the arena after " << i
                      << " longs!\n";
            break;
        }
    }

    return 0;
}
