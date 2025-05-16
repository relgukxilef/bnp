#include "test.h"

#include <iostream>
#include <exception>

int main(int argc, const char *argv[]) {
    auto test = bnp::tests;
    while (test) {
        try {
            test->run();
        } catch(std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        test = test->next;
    }
}
