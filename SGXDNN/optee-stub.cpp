
// This file is used to resolve `undefined reference` error

#include <cstdlib>
#include <cstdio>

namespace std {

void __attribute__((noreturn)) __throw_length_error(const char *msg) {
    std::puts(msg);
    std::abort();
}

void __attribute__((noreturn)) __throw_bad_function_call() {
    std::puts("abort in __throw_bad_function_call()");
    std::abort();
}

} // namespace std
