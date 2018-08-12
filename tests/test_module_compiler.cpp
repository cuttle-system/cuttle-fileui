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
    using namespace boost::filesystem;

    {
        path tmp = create_tmp();
        create_directory(tmp / "fooModule");
        create_directory(tmp / "barModule");
        std::list<path> search_path = {tmp};
        cuttle::fileui::compile_state state {search_path};
        auto result = cuttle::fileui::search_module("fooModule", state);
        AssertEqual(result, tmp / "fooModule", "Module path");
    }
    {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "fooModule");
        create_directories(tmp / "modulesB" / "bazModule");
        std::list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        cuttle::fileui::compile_state state {search_path};
        AssertThrows(cuttle::fileui::module_duplicate_error, {
            cuttle::fileui::search_module("fooModule", state);
        });
    }
    {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "barModule");
        create_directories(tmp / "modulesB" / "bazModule");
        std::list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        cuttle::fileui::compile_state state {search_path};
        auto result = cuttle::fileui::search_module("bazModule", state);
        AssertEqual(result, tmp / "modulesB" / "bazModule", "Module path");
    }
}

inline void test_can_compile_function() {
    using namespace boost::filesystem;

    {
        path tmp = create_tmp();

        create_directories(tmp / "fooModule" / "fooFunction");

        path function_path = tmp / "fooModule" / "fooFunction";
        path module_path = tmp / "fooModule";
        path compiled_function_path = cuttle::fileui::get_compiled_function_path(function_path);
        std::list<path> search_path = {tmp};

        cuttle::fileui::compile_state state {search_path};
        cuttle::fileui::compile_function(function_path, module_path, state);

        AssertTrue(is_directory(compiled_function_path), "Foo function exists");
    }
}

inline void test_can_compile_functions() {
    using namespace boost::filesystem;

    {
        path tmp = create_tmp();

        create_directories(tmp / "fooModule" / "fooFunction");
        create_directories(tmp / "fooModule" / "barFunction");
        create_directories(tmp / "fooModule" / "bazFunction");

        create_directories(tmp / "barModule" / "bazFunction");
        create_directories(tmp / "barModule" / "quxFunction");

        path module_path = tmp / "fooModule";
        path compiled_module_path = cuttle::fileui::get_compiled_module_path(module_path);
        std::list<path> search_path = {tmp};

        cuttle::fileui::compile_state state {search_path};
        cuttle::fileui::compile_functions(module_path, state);

        AssertTrue(is_directory(compiled_module_path / "fooFunction"), "Foo function exists");
        AssertTrue(is_directory(compiled_module_path / "barFunction"), "Bar function exists");
        AssertTrue(is_directory(compiled_module_path / "bazFunction"), "Baz function exists");
    }
}

void run_module_compiler_tests() {
	TESTCASE
	test_can_search_module();
    test_can_compile_function();
    test_can_compile_functions();
}
