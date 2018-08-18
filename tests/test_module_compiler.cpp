#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "test_utils.hpp"
#include "module_compiler.hpp"
#include "module_duplicate_error.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace cuttle::fileui;

BOOST_AUTO_TEST_SUITE(search_module_suite)

    BOOST_AUTO_TEST_CASE(case1)
    {
        path tmp = create_tmp();
        create_directory(tmp / "fooModule");
        create_directory(tmp / "barModule");
        list<path> search_path = {tmp};
        compile_state state {search_path};
        auto result = search_module("fooModule", state);
        BOOST_CHECK_EQUAL(result, tmp / "fooModule");
    }

    BOOST_AUTO_TEST_CASE(case2)
    {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "fooModule");
        create_directories(tmp / "modulesB" / "bazModule");
        list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        compile_state state {search_path};
        BOOST_CHECK_THROW(search_module("fooModule", state), module_duplicate_error);
    }

    BOOST_AUTO_TEST_CASE(case3)
    {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "barModule");
        create_directories(tmp / "modulesB" / "bazModule");
        list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        compile_state state {search_path};
        auto result = search_module("bazModule", state);
        BOOST_CHECK_EQUAL(result, tmp / "modulesB" / "bazModule");
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(compile_function_suite)

    BOOST_AUTO_TEST_CASE(case1)
    {
        path tmp = create_tmp();

        create_directories(tmp / "fooModule" / "fooFunction");

        path function_path = tmp / "fooModule" / "fooFunction";
        path module_path = tmp / "fooModule";
        path compiled_function_path = get_compiled_function_path(function_path);
        list<path> search_path = {tmp};

        compile_state state {search_path};
        compile_function(function_path, module_path, state);

        BOOST_CHECK(is_directory(compiled_function_path));
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(compile_functions_suite)

    BOOST_AUTO_TEST_CASE(case1)
    {
        path tmp = create_tmp();

        create_directories(tmp / "fooModule" / "fooFunction");
        create_directories(tmp / "fooModule" / "barFunction");
        create_directories(tmp / "fooModule" / "bazFunction");

        create_directories(tmp / "barModule" / "bazFunction");
        create_directories(tmp / "barModule" / "quxFunction");

        path module_path = tmp / "fooModule";
        path compiled_module_path = get_compiled_module_path(module_path);
        list<path> search_path = {tmp};

        compile_state state{search_path};
        compile_functions(module_path, state);

        BOOST_CHECK(is_directory(compiled_module_path / "fooFunction"));
        BOOST_CHECK(is_directory(compiled_module_path / "barFunction"));
        BOOST_CHECK(is_directory(compiled_module_path / "bazFunction"));
    }

BOOST_AUTO_TEST_SUITE_END()