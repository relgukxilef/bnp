#pragma once

#include <sstream>
#include <stdexcept>

namespace bnp {
    using namespace std;

    extern const struct test *tests;
    struct test {
        test(void (*run)()) : run(run) {
            next = tests;
            tests = this;
        }
        void (*run)();
        test const *next;
    };

    template<typename T>
    struct checked_t {
        checked_t(const T &t) : t(t) {}
        const T t;

        template<typename O>
        bool check(bool condition, const O &o, const char *op) { 
            if (condition) 
                return true;
            stringstream stream;
            stream << t << ' ' << op << ' ' << o;
            throw runtime_error(std::move(stream.str()));
        }
        template<typename O>
        bool operator ==(const O &o) { return check(t == o, o, "!="); }

    };

    template<typename T>
    checked_t<T> checked(const T &t) {
        return checked_t<T>(t);
    }
}
