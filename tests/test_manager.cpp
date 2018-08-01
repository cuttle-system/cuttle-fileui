#include <iostream>
#include "test_file_access.hpp"
#include "test_module_compiler.hpp"

void run_tests() {
    run_file_access_tests();
    run_module_compiler_tests();
}