#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <thread>
#include "test_utils.hpp"
#include "compiler.hpp"
#include "module_duplicate_error.hpp"

using namespace std;
using namespace boost::filesystem;
using namespace cuttle::fileui;
using namespace cuttle;

BOOST_AUTO_TEST_SUITE(search_module_suite)

    BOOST_AUTO_TEST_CASE(case1) {
        path tmp = create_tmp();
        create_directory(tmp / "fooModule");
        create_directory(tmp / "barModule");
        list<path> search_path = {tmp};
        compile_state state {search_path, {}};
        auto result = search_module(state, "fooModule");
        BOOST_CHECK_EQUAL(result, tmp / "fooModule");
    }

    BOOST_AUTO_TEST_CASE(case2) {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "fooModule");
        create_directories(tmp / "modulesB" / "bazModule");
        list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        compile_state state {search_path, {}};
        BOOST_CHECK_THROW(search_module(state, "fooModule"), module_duplicate_error);
    }

    BOOST_AUTO_TEST_CASE(case3) {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "barModule");
        create_directories(tmp / "modulesB" / "bazModule");
        list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        compile_state state {search_path, {}};
        auto result = search_module(state, "bazModule");
        BOOST_CHECK_EQUAL(result, tmp / "modulesB" / "bazModule");
    }

BOOST_AUTO_TEST_SUITE_END()

struct compile_file_suite_fixture {
    compile_state state = {{}, {}};
};

BOOST_FIXTURE_TEST_SUITE(compile_file_suite, compile_file_suite_fixture)

    BOOST_AUTO_TEST_CASE(case1) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.cutc";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.cutvm";

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream compiled_cutc_file(compiled_cutc_path.string());
        std::string compiled_cutc_src((std::istreambuf_iterator<char>(compiled_cutc_file)),
                               std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(compiled_cutc_src, "b s to\n"
                                             "b s .\n"
                                             "b s cutc-tokenizer\n"
                                             "b i 1\n"
                                             "c 3 0 array\n"
                                             "b s .\n"
                                             "b s cutvm\n"
                                             "b i 1\n"
                                             "c 3 0 array\n"
                                             "c 3 0 array");

        std::ifstream compiled_file(compiled_file_path.string());
        std::string compiled_file_src((std::istreambuf_iterator<char>(compiled_file)),
                                      std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(compiled_file_src, "b s normal_string\n"
                                             "b s ->\n"
                                             "b s \\'\n"
                                             "b s \\'\n"
                                             "c 3 0 array\n"
                                             "c 2 0 array");

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                    std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "b s normal_string\n"
                                           "b s ->\n"
                                           "b s \\'\n"
                                           "b s \\'\n"
                                           "c 3 0 array\n"
                                           "c 2 0 array");
    }

    BOOST_AUTO_TEST_CASE(case2) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.A";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.B";

        path A_tokenizer_cutc_path = tmp / "A.1" / "tokenizer" / "rules.cutc.cutc";
        path A_tokenizer_path = tmp / "A.1" / "tokenizer" / "rules.cutc";

        create_directories(tmp / "A.1" / "tokenizer");

        std::ofstream A_tokenizer_cutc_file(A_tokenizer_cutc_path.string());
        A_tokenizer_cutc_file << "just 'cutc-tokenizer'.1";
        A_tokenizer_cutc_file.close();

        std::ofstream A_tokenizer_file(A_tokenizer_path.string());
        A_tokenizer_file << "normal_string \"'\" -> \"'\"\n"
                            "formatted_string \"\\\"\" -> \"\\\"\"";
        A_tokenizer_file.close();

        path B_tokenizer_cutc_path = tmp / "B.1" / "tokenizer" / "rules.cutc.cutc";
        path B_tokenizer_path = tmp / "B.1" / "tokenizer" / "rules.cutc";

        create_directories(tmp / "B.1" / "tokenizer");

        std::ofstream B_tokenizer_cutc_file(B_tokenizer_cutc_path.string());
        B_tokenizer_cutc_file << "just 'cutc-tokenizer'.1";
        B_tokenizer_cutc_file.close();

        std::ofstream B_tokenizer_file(B_tokenizer_path.string());
        B_tokenizer_file << "normal_string _ -> _\n"
                            "formatted_string | -> |";
        B_tokenizer_file.close();

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "A.1 to B.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << "'foo' '' \"\\n\"";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                      std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "|foo|\n||\n|\\n|");
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(compile_file_cache_read_suite, compile_file_suite_fixture)

    BOOST_AUTO_TEST_CASE(case1) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.cutc";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.cutvm";

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        cutc_file.close();

        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
        compiled_cutc_file << "b s to\n"
                              "b s .\n"
                              "b s cutc-tokenizer\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "b s .\n"
                              "b s cutvm\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "c 3 0 array";
        compiled_cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        std::ofstream compiled_file(compiled_file_path.string());
        compiled_file << "b s normal_string\n"
                         "b s ->\n"
                         "b s '\n"
                         "b s '\n"
                         "c 3 0 array\n"
                         "c 2 0 array";
        compiled_file.close();

        auto compiled_file_last_modified = last_write_time(compiled_file_path);
        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        BOOST_CHECK_EQUAL(compiled_file_last_modified, last_write_time(compiled_file_path));
        BOOST_CHECK_EQUAL(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                      std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "b s normal_string\n"
                                             "b s ->\n"
                                             "b s \\'\n"
                                             "b s \\'\n"
                                             "c 3 0 array\n"
                                             "c 2 0 array");
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(compile_file_cache_compilation_suite, compile_file_suite_fixture)

    BOOST_AUTO_TEST_CASE(cached_files_not_recompiled) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.cutc";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.cutvm";

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        cutc_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
        compiled_cutc_file << "b s to\n"
                              "b s .\n"
                              "b s cutc-tokenizer\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "b s .\n"
                              "b s cutvm\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "c 3 0 array";
        compiled_cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream compiled_file(compiled_file_path.string());
        compiled_file << "b s normal_string\n"
                         "b s ->\n"
                         "b s '\n"
                         "b s '\n"
                         "c 3 0 array\n"
                         "c 2 0 array";
        compiled_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto compiled_file_last_modified = last_write_time(compiled_file_path);
        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        BOOST_CHECK_EQUAL(compiled_file_last_modified, last_write_time(compiled_file_path));
        BOOST_CHECK_EQUAL(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
    }

    BOOST_AUTO_TEST_CASE(cached_files_recompiled1) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.cutc";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.cutvm";

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        cutc_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
        compiled_cutc_file << "b s to\n"
                              "b s .\n"
                              "b s cutc-tokenizer\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "b s .\n"
                              "b s cutvm\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "c 3 0 array";
        compiled_cutc_file.close();

        std::ofstream compiled_file(compiled_file_path.string());
        compiled_file << "b s normal_string\n"
                         "b s ->\n"
                         "b s '\n"
                         "b s '\n"
                         "c 3 0 array\n"
                         "c 2 0 array";
        compiled_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto compiled_file_last_modified = last_write_time(compiled_file_path);
        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        BOOST_CHECK_LT(compiled_file_last_modified, last_write_time(compiled_file_path));
        BOOST_CHECK_LT(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
    }

    BOOST_AUTO_TEST_CASE(cached_files_recompiled2) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.cutc";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.cutvm";

        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
        compiled_cutc_file << "b s to\n"
                              "b s .\n"
                              "b s cutc-tokenizer\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "b s .\n"
                              "b s cutvm\n"
                              "b i 1\n"
                              "c 3 0 array\n"
                              "c 3 0 array";
        compiled_cutc_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::ofstream compiled_file(compiled_file_path.string());
        compiled_file << "b s normal_string\n"
                         "b s ->\n"
                         "b s '\n"
                         "b s '\n"
                         "c 3 0 array\n"
                         "c 2 0 array";
        compiled_file.close();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto compiled_file_last_modified = last_write_time(compiled_file_path);
        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        BOOST_CHECK_LT(compiled_file_last_modified, last_write_time(compiled_file_path));
        BOOST_CHECK_LT(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
    }

BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_SUITE(compile_function_suite)
//
//    BOOST_AUTO_TEST_CASE(case1)
//    {
//        path tmp = create_tmp();
//
//        create_directories(tmp / "fooModule" / "fooFunction");
//
//        path function_path = tmp / "fooModule" / "fooFunction";
//        path module_path = tmp / "fooModule";
//        path compiled_function_path = get_compiled_function_path(function_path);
//        list<path> search_path = {tmp};
//
//        compile_state state {search_path};
//        compile_function(function_path, state);
//
//        BOOST_CHECK(is_directory(compiled_function_path));
//    }
//
//BOOST_AUTO_TEST_SUITE_END()
//
//BOOST_AUTO_TEST_SUITE(compile_functions_suite)
//
//    BOOST_AUTO_TEST_CASE(case1)
//    {
//        path tmp = create_tmp();
//
//        create_directories(tmp / "fooModule" / "fooFunction");
//        create_directories(tmp / "fooModule" / "barFunction");
//        create_directories(tmp / "fooModule" / "bazFunction");
//
//        create_directories(tmp / "barModule" / "bazFunction");
//        create_directories(tmp / "barModule" / "quxFunction");
//
//        path module_path = tmp / "fooModule";
//        path compiled_module_path = get_compiled_module_path(module_path);
//        list<path> search_path = {tmp};
//
//        compile_state state{search_path};
//        compile_functions(module_path, state);
//
//        BOOST_CHECK(is_directory(compiled_module_path / "fooFunction"));
//        BOOST_CHECK(is_directory(compiled_module_path / "barFunction"));
//        BOOST_CHECK(is_directory(compiled_module_path / "bazFunction"));
//    }
//
//BOOST_AUTO_TEST_SUITE_END()