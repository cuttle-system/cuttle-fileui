#include <iostream>
#include <fstream>
#include <string>
#include "test.hpp"
#include "test_file_access.hpp"

using namespace std;

inline void test_can_read_and_write_files() {
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

void run_file_access_tests() {
    TESTCASE
    test_can_read_and_write_files();
}