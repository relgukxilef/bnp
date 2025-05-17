#include "test.h"

#include <iostream>
#include <exception>

int main(int argc, const char *argv[]) {
    int total = 0, passed = 0;
    
    auto test = bnp::tests;
    while (test) {
        total++;
        try {
            test->run();
            passed++;
        } catch(std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        test = test->next;
    }

    std::cout << passed << " / " << total << " tests passed." << std::endl;

    if (passed == 0 || passed != total)
        return -1;

    return 0;
}
