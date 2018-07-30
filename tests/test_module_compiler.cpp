#include <iostream>
#include <fstream>
//#include <filesystem>
//#include <experimental/filesystem>
#include "test.hpp"
#include "test_module_compiler.hpp"

using namespace std;

inline void test_can_compile_options() {
	AssertTrue(false, "Not implemented");
    string data = "This is a test data";
    ofstream os("test_file_access.txt", ios::out);
    os << data;
    os.close();

    string str;
    ifstream is("test_file_access.txt", ios::in);
    getline(is, str);
    AssertTrue(str == data, "Name constructor");
    is.close();
}

void run_module_compiler_tests() {
	TESTCASE
	test_can_compile_options();
}