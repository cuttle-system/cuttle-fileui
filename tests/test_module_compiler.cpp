#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "test.hpp"
#include "module_compiler.hpp"
#include "test_module_compiler.hpp"
#include "module_duplicate_error.hpp"

using namespace std;

inline boost::filesystem::path create_tmp() {
    using namespace boost::filesystem;
    path tmp = current_path() / "tmp" / unique_path();
    create_directories(tmp);
    return tmp;
}

inline void test_can_search_module() {
    using namespace cuttle::fileui;

    {
        boost::filesystem::path tmp = create_tmp();
        boost::filesystem::create_directory(tmp / "fooModule");
        boost::filesystem::create_directory(tmp / "barModule");
        std::list<boost::filesystem::path> search_path = {tmp};
        auto result = search_module("fooModule", search_path);
        AssertEqual(result, tmp / "fooModule", "Module path");
    }
    {
        boost::filesystem::path tmp = create_tmp();
        boost::filesystem::create_directories(tmp / "modulesA" / "fooModule");
        boost::filesystem::create_directories(tmp / "modulesA" / "barModule");
        boost::filesystem::create_directories(tmp / "modulesB" / "fooModule");
        boost::filesystem::create_directories(tmp / "modulesB" / "bazModule");
        std::list<boost::filesystem::path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        AssertThrows(module_duplicate_error, {
            search_module("fooModule", search_path);
        });
    }
    {
        boost::filesystem::path tmp = create_tmp();
        boost::filesystem::create_directories(tmp / "modulesA" / "fooModule");
        boost::filesystem::create_directories(tmp / "modulesA" / "barModule");
        boost::filesystem::create_directories(tmp / "modulesB" / "barModule");
        boost::filesystem::create_directories(tmp / "modulesB" / "bazModule");
        std::list<boost::filesystem::path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        auto result = search_module("bazModule", search_path);
        AssertEqual(result, tmp / "modulesB" / "bazModule", "Module path");
    }
}

void run_module_compiler_tests() {
	TESTCASE
	test_can_search_module();
}