#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <chrono>
#include <thread>
#include "test_utils.hpp"
#include "compiler.hpp"
#include "module_duplicate_error.hpp"
#include "fileui_module.hpp"

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
        compile_state_t state{search_path, {}};
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
        compile_state_t state{search_path, {}};
        BOOST_CHECK_THROW(search_module(state, "fooModule"), module_duplicate_error);
    }

    BOOST_AUTO_TEST_CASE(case3) {
        path tmp = create_tmp();
        create_directories(tmp / "modulesA" / "fooModule");
        create_directories(tmp / "modulesA" / "barModule");
        create_directories(tmp / "modulesB" / "barModule");
        create_directories(tmp / "modulesB" / "bazModule");
        list<path> search_path = {tmp / "modulesA", tmp / "modulesB"};
        compile_state_t state{search_path, {}};
        auto result = search_module(state, "bazModule");
        BOOST_CHECK_EQUAL(result, tmp / "modulesB" / "bazModule");
    }

BOOST_AUTO_TEST_SUITE_END()

struct compile_file_suite_fixture {
    compile_state_t state = {{},
                             {}};
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
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm-cache'.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream compiled_cutc_file(compiled_cutc_path.string());
        std::string compiled_cutc_src((std::istreambuf_iterator<char>(compiled_cutc_file)),
                                      std::istreambuf_iterator<char>());

//        BOOST_CHECK_EQUAL(compiled_cutc_src, "b s to\n"
//                                             "b s .\n"
//                                             "b s cutc-tokenizer\n"
//                                             "b i 1\n"
//                                             "c 3 0 array\n"
//                                             "b s .\n"
//                                             "b s cutvm-cache\n"
//                                             "b i 1\n"
//                                             "c 3 0 array\n"
//                                             "c 3 0 array");

        std::ifstream compiled_file(compiled_file_path.string());
        std::string compiled_file_src((std::istreambuf_iterator<char>(compiled_file)),
                                      std::istreambuf_iterator<char>());

//        BOOST_CHECK_EQUAL(compiled_file_src, "b s normal_string\n"
//                                             "b s ->\n"
//                                             "b s '\n"
//                                             "b s '\n"
//                                             "c 3 0 array\n"
//                                             "c 2 0 array");

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                    std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "b s f\n"
                                           "b s atom\n"
                                           "b s normal_string\n"
                                           "c 2 0 array\n"
                                           "b s f\n"
                                           "b s atom\n"
                                           "b s ->\n"
                                           "c 2 0 array\n"
                                           "b s string\n"
                                           "b s '\n"
                                           "c 2 0 array\n"
                                           "b s string\n"
                                           "b s '\n"
                                           "c 2 0 array\n"
                                           "c 4 0 array\n"
                                           "c 3 0 array");
    }

    BOOST_AUTO_TEST_CASE(case2) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.A";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.B";

        path A_tokenizer_cutc_path = tmp / "A.1" / "parser" / "tokenizer" / "rules.cutl.cutc";
        path A_tokenizer_path = tmp / "A.1" / "parser" / "tokenizer" / "rules.cutl";

        create_directories(tmp / "A.1" / "parser" / "tokenizer");

        std::ofstream A_cutroot((tmp / "A.1" / ".cutroot").string());
        A_cutroot << "";
        A_cutroot.close();

        std::ofstream A_tokenizer_cutc_file(A_tokenizer_cutc_path.string());
        A_tokenizer_cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        A_tokenizer_cutc_file.close();

        std::ofstream A_tokenizer_file(A_tokenizer_path.string());
        A_tokenizer_file << "normal_string \"'\" -> \"'\"\n"
                            "formatted_string \"\\\"\" -> \"\\\"\"";
        A_tokenizer_file.close();

        path B_tokenizer_cutc_path = tmp / "B.1" / "parser" / "tokenizer" / "rules.cutl.cutc";
        path B_tokenizer_path = tmp / "B.1" / "parser" / "tokenizer" / "rules.cutl";

        create_directories(tmp / "B.1" / "parser" / "tokenizer");

        std::ofstream B_cutroot((tmp / "B.1" / ".cutroot").string());
        B_cutroot << "";
        B_cutroot.close();


        std::ofstream B_tokenizer_cutc_file(B_tokenizer_cutc_path.string());
        B_tokenizer_cutc_file << "'cutc-tokenizer'.1 to 'cutvm'.1";
        B_tokenizer_cutc_file.close();

        std::ofstream B_tokenizer_file(B_tokenizer_path.string());
        B_tokenizer_file << "normal_string '_' -> '_'\n"
                            "formatted_string '|' -> '|'";
        B_tokenizer_file.close();

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "A.1 to B.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"('foo' '' "\n")";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                    std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "|foo|\n||\n|\\n|");
    }

    BOOST_AUTO_TEST_CASE(case3) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.A";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.B";

        path A_parser_path = tmp / "A.1" / "parser";

        auto A_parser_foo_path = A_parser_path / "functions" / "1_foo";
        create_directories(A_parser_foo_path);

        std::ofstream A_cutroot((tmp / "A.1" / ".cutroot").string());
        A_cutroot << "";
        A_cutroot.close();

        std::ofstream A_parser_foo_cutc_file((A_parser_foo_path / "rules.cutl.cutc").string());
        A_parser_foo_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_foo_cutc_file.close();

        std::ofstream A_parser_foo_file((A_parser_foo_path / "rules.cutl").string());
        A_parser_foo_file << "name 'foo'\n"
                             "type infix\n"
                             "args_number 2\n"
                             "executes_before last_func_id";
        A_parser_foo_file.close();

        auto A_parser_plus_path = A_parser_path / "functions" / "2_plus";
        create_directories(A_parser_plus_path);

        std::ofstream A_parser_plus_cutc_file((A_parser_plus_path / "rules.cutl.cutc").string());
        A_parser_plus_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_plus_cutc_file.close();

        std::ofstream A_parser_plus_file((A_parser_plus_path / "rules.cutl").string());
        A_parser_plus_file << "name '+'\n"
                              "type infix\n"
                              "args_number 2\n"
                              "executes_before func_id 'foo'";
        A_parser_plus_file.close();

        auto A_parser_factorial_path = A_parser_path / "functions" / "3_factorial";
        create_directories(A_parser_factorial_path);

        std::ofstream A_parser_factorial_cutc_file((A_parser_factorial_path / "rules.cutl.cutc").string());
        A_parser_factorial_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_factorial_cutc_file.close();

        std::ofstream A_parser_factorial_file((A_parser_factorial_path / "rules.cutl").string());
        A_parser_factorial_file << "name '!'\n"
                                   "type postfix\n"
                                   "args_number 1\n"
                                   "executes_before func_id 'foo'";
        A_parser_factorial_file.close();

        path B_parser_path = tmp / "B.1" / "parser";

        auto B_parser_foo_path = B_parser_path / "functions" / "1_foo";
        create_directories(B_parser_foo_path);

        std::ofstream B_cutroot((tmp / "B.1" / ".cutroot").string());
        B_cutroot << "";
        B_cutroot.close();

        std::ofstream B_parser_foo_cutc_file((B_parser_foo_path / "rules.cutl.cutc").string());
        B_parser_foo_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_foo_cutc_file.close();

        std::ofstream B_parser_foo_file((B_parser_foo_path / "rules.cutl").string());
        B_parser_foo_file << "name 'foo'\n"
                             "type infix\n"
                             "args_number 2\n"
                             "executes_before last_func_id";
        B_parser_foo_file.close();

        auto B_parser_plus_path = B_parser_path / "functions" / "2_plus";
        create_directories(B_parser_plus_path);

        std::ofstream B_parser_plus_cutc_file((B_parser_plus_path / "rules.cutl.cutc").string());
        B_parser_plus_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_plus_cutc_file.close();

        std::ofstream B_parser_plus_file((B_parser_plus_path / "rules.cutl").string());
        B_parser_plus_file << "name '+'\n"
                              "type prefix\n"
                              "args_number 2\n"
                              "executes_before func_id 'foo'";
        B_parser_plus_file.close();

        auto B_parser_factorial_path = B_parser_path / "functions" / "3_factorial";
        create_directories(B_parser_factorial_path);

        std::ofstream B_parser_factorial_cutc_file((B_parser_factorial_path / "rules.cutl.cutc").string());
        B_parser_factorial_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_factorial_cutc_file.close();

        std::ofstream B_parser_factorial_file((B_parser_factorial_path / "rules.cutl").string());
        B_parser_factorial_file << "name '!'\n"
                                   "type prefix\n"
                                   "args_number 1\n"
                                   "executes_before func_id 'foo'";
        B_parser_factorial_file.close();

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "A.1 to B.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << "1 + 3 foo 2 !";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                    std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "+ 1 3 foo ! 2");
    }

    BOOST_AUTO_TEST_CASE(case4) {
        path tmp = create_tmp();
        path file_path = tmp / "foo.A";
        path cutc_path = file_path.string() + ".cutc";
        path compiled_file_path = file_path.string() + ".cutvm";
        path compiled_cutc_path = cutc_path.string() + ".cutvm";
        path output_file_path = tmp / "foo.B";

        path A_parser_path = tmp / "A.1" / "parser";

        auto A_parser_foo_path = A_parser_path / "functions" / "1_foo";
        create_directories(A_parser_foo_path);

        std::ofstream A_cutroot((tmp / "A.1" / ".cutroot").string());
        A_cutroot << "";
        A_cutroot.close();

        std::ofstream A_parser_foo_cutc_file((A_parser_foo_path / "rules.cutl.cutc").string());
        A_parser_foo_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_foo_cutc_file.close();

        std::ofstream A_parser_foo_file((A_parser_foo_path / "rules.cutl").string());
        A_parser_foo_file << "name 'foo'\n"
                             "type infix\n"
                             "args_number 2\n"
                             "executes_before last_func_id";
        A_parser_foo_file.close();

        auto A_parser_plus_path = A_parser_path / "functions" / "2_plus";
        create_directories(A_parser_plus_path);

        std::ofstream A_parser_plus_cutc_file((A_parser_plus_path / "rules.cutl.cutc").string());
        A_parser_plus_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_plus_cutc_file.close();

        std::ofstream A_parser_plus_file((A_parser_plus_path / "rules.cutl").string());
        A_parser_plus_file << "name '+'\n"
                              "type infix\n"
                              "args_number 2\n"
                              "executes_before func_id 'foo'";
        A_parser_plus_file.close();

        auto A_parser_factorial_path = A_parser_path / "functions" / "3_factorial";
        create_directories(A_parser_factorial_path);

        std::ofstream A_parser_factorial_cutc_file((A_parser_factorial_path / "rules.cutl.cutc").string());
        A_parser_factorial_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        A_parser_factorial_cutc_file.close();

        std::ofstream A_parser_factorial_file((A_parser_factorial_path / "rules.cutl").string());
        A_parser_factorial_file << "name '!'\n"
                                   "type postfix\n"
                                   "args_number 1\n"
                                   "executes_before func_id 'foo'";
        A_parser_factorial_file.close();

        path B_parser_path = tmp / "B.1" / "parser";

        auto B_parser_foo_path = B_parser_path / "functions" / "1_foo";
        create_directories(B_parser_foo_path);

        std::ofstream B_cutroot((tmp / "B.1" / ".cutroot").string());
        B_cutroot << "";
        B_cutroot.close();

        std::ofstream B_parser_foo_cutc_file((B_parser_foo_path / "rules.cutl.cutc").string());
        B_parser_foo_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_foo_cutc_file.close();

        std::ofstream B_parser_foo_file((B_parser_foo_path / "rules.cutl").string());
        B_parser_foo_file << "name 'foo'\n"
                             "type prefix\n"
                             "args_number 2\n"
                             "executes_before last_func_id";
        B_parser_foo_file.close();

        auto B_parser_plus_path = B_parser_path / "functions" / "2_plus";
        create_directories(B_parser_plus_path);

        std::ofstream B_parser_plus_cutc_file((B_parser_plus_path / "rules.cutl.cutc").string());
        B_parser_plus_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_plus_cutc_file.close();

        std::ofstream B_parser_plus_file((B_parser_plus_path / "rules.cutl").string());
        B_parser_plus_file << "name '+'\n"
                              "type prefix\n"
                              "args_number 2\n"
                              "executes_before func_id 'foo'";
        B_parser_plus_file.close();

        auto B_parser_plus_plus_path = B_parser_path / "functions" / "3_plus_plus";
        create_directories(B_parser_plus_plus_path);

        std::ofstream B_parser_plus_plus_cutc_file((B_parser_plus_plus_path / "rules.cutl.cutc").string());
        B_parser_plus_plus_cutc_file << "'cutc-parser'.1 to 'cutvm'.1";
        B_parser_plus_plus_cutc_file.close();

        std::ofstream B_parser_plus_plus_file((B_parser_plus_plus_path / "rules.cutl").string());
        B_parser_plus_plus_file << "name '++'\n"
                                   "type prefix\n"
                                   "args_number 1\n"
                                   "executes_before last_func_id";
        B_parser_plus_plus_file.close();

        path A_B_translator_path = tmp / "A.1" / "translators" / "B.1";

        auto A_B_translator_bar_path = A_B_translator_path / "functions" / "1_bar";
        create_directories(A_B_translator_bar_path);

        std::ofstream A_B_translator_bar_pattern_cutc_file((A_B_translator_bar_path / "pattern.A.cutc").string());
        A_B_translator_bar_pattern_cutc_file << "just 'A'.1";
        A_B_translator_bar_pattern_cutc_file.close();

        std::ofstream A_B_translator_bar_pattern_file((A_B_translator_bar_path / "pattern.A").string());
        A_B_translator_bar_pattern_file << "0pf_func0 foo 0p_a0 !";
        A_B_translator_bar_pattern_file.close();

        std::ofstream A_B_translator_bar_output_cutc_file((A_B_translator_bar_path / "output.B.cutc").string());
        A_B_translator_bar_output_cutc_file << "just 'B'.1";
        A_B_translator_bar_output_cutc_file.close();

        std::ofstream A_B_translator_bar_output_file((A_B_translator_bar_path / "output.B").string());
        A_B_translator_bar_output_file << "++ foo 0p_a0 0pf_func0";
        A_B_translator_bar_output_file.close();

        std::ofstream cutc_file(cutc_path.string());
        cutc_file << "A.1 to B.1";
        cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << "1 + 3 foo 2 !";
        src_file.close();

        state.search_path = {tmp};
        compile_file(state, file_path, compiled_file_path, output_file_path);

        std::ifstream output_file(output_file_path.string());
        std::string output_file_src((std::istreambuf_iterator<char>(output_file)),
                                    std::istreambuf_iterator<char>());

        BOOST_CHECK_EQUAL(output_file_src, "++ foo 2 + 1 3");
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
        cutc_file << "'cutc-tokenizer'.1 to 'cutvm-cache'.1";
        cutc_file.close();

        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
        compiled_cutc_file << "b s f\n"
                              "b s atom\n"
                              "b s to\n"
                              "c 2 0 array\n"

                              "b s f\n"
                              "b s atom\n"
                              "b s .\n"
                              "c 2 0 array\n"
                              "b s string\n"
                              "b s cutc-tokenizer\n"
                              "c 2 0 array\n"
                              "b s number\n"
                              "b s 1\n"
                              "c 2 0 array\n"
                              "c 4 0 array\n"

                              "b s f\n"
                              "b s atom\n"
                              "b s .\n"
                              "c 2 0 array\n"
                              "b s string\n"
                              "b s cutvm-cache\n"
                              "c 2 0 array\n"
                              "b s number\n"
                              "b s 1\n"
                              "c 2 0 array\n"
                              "c 4 0 array\n"

                              "c 4 0 array";
        compiled_cutc_file.close();

        std::ofstream src_file(file_path.string());
        src_file << R"(normal_string "'" -> "'")";
        src_file.close();

        std::ofstream compiled_file(compiled_file_path.string());
        compiled_file << "b s f\n"
                         "b s atom\n"
                         "b s normal_string\n"
                         "c 2 0 array\n"

                         "b s f\n"
                         "b s atom\n"
                         "b s ->\n"
                         "c 2 0 array\n"
                         "b s string\n"
                         "b s '\n"
                         "c 2 0 array\n"
                         "b s string\n"
                         "b s '\n"
                         "c 2 0 array\n"
                         "c 4 0 array\n"

                         "c 3 0 array";
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

        BOOST_CHECK_EQUAL(output_file_src, "b s f\n"
                                           "b s atom\n"
                                           "b s normal_string\n"
                                           "c 2 0 array\n"

                                           "b s f\n"
                                           "b s atom\n"
                                           "b s ->\n"
                                           "c 2 0 array\n"
                                           "b s string\n"
                                           "b s '\n"
                                           "c 2 0 array\n"
                                           "b s string\n"
                                           "b s '\n"
                                           "c 2 0 array\n"
                                           "c 4 0 array\n"

                                           "c 3 0 array");
    }

BOOST_AUTO_TEST_SUITE_END()

//BOOST_FIXTURE_TEST_SUITE(compile_file_cache_compilation_suite, compile_file_suite_fixture)
//
//    BOOST_AUTO_TEST_CASE(cached_files_not_recompiled) {
//        path tmp = create_tmp();
//        path file_path = tmp / "foo.cutc";
//        path cutc_path = file_path.string() + ".cutc";
//        path compiled_file_path = file_path.string() + ".cutvm";
//        path compiled_cutc_path = cutc_path.string() + ".cutvm";
//        path output_file_path = tmp / "foo.cutvm";
//
//        std::ofstream cutc_file(cutc_path.string());
//        cutc_file << "'cutc-tokenizer'.1 to 'cutvm-cache'.1";
//        cutc_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
//        compiled_cutc_file << "b s to\n"
//                              "b s .\n"
//                              "b s cutc-tokenizer\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "b s .\n"
//                              "b s cutvm-cache\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "c 3 0 array";
//        compiled_cutc_file.close();
//
//        std::ofstream src_file(file_path.string());
//        src_file << R"(normal_string "'" -> "'")";
//        src_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream compiled_file(compiled_file_path.string());
//        compiled_file << "b s normal_string\n"
//                         "b s ->\n"
//                         "b s '\n"
//                         "b s '\n"
//                         "c 3 0 array\n"
//                         "c 2 0 array";
//        compiled_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        auto compiled_file_last_modified = last_write_time(compiled_file_path);
//        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);
//
//        state.search_path = {tmp};
//        compile_file(state, file_path, compiled_file_path, output_file_path);
//
//        BOOST_CHECK_EQUAL(compiled_file_last_modified, last_write_time(compiled_file_path));
//        BOOST_CHECK_EQUAL(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
//    }
//
//    BOOST_AUTO_TEST_CASE(cached_files_recompiled1) {
//        path tmp = create_tmp();
//        path file_path = tmp / "foo.cutc";
//        path cutc_path = file_path.string() + ".cutc";
//        path compiled_file_path = file_path.string() + ".cutvm";
//        path compiled_cutc_path = cutc_path.string() + ".cutvm";
//        path output_file_path = tmp / "foo.cutvm";
//
//        std::ofstream cutc_file(cutc_path.string());
//        cutc_file << "'cutc-tokenizer'.1 to 'cutvm-cache'.1";
//        cutc_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
//        compiled_cutc_file << "b s to\n"
//                              "b s .\n"
//                              "b s cutc-tokenizer\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "b s .\n"
//                              "b s cutvm-cache\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "c 3 0 array";
//        compiled_cutc_file.close();
//
//        std::ofstream compiled_file(compiled_file_path.string());
//        compiled_file << "b s normal_string\n"
//                         "b s ->\n"
//                         "b s '\n"
//                         "b s '\n"
//                         "c 3 0 array\n"
//                         "c 2 0 array";
//        compiled_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream src_file(file_path.string());
//        src_file << R"(normal_string "'" -> "'")";
//        src_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        auto compiled_file_last_modified = last_write_time(compiled_file_path);
//        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);
//
//        state.search_path = {tmp};
//        compile_file(state, file_path, compiled_file_path, output_file_path);
//
//        BOOST_CHECK_LT(compiled_file_last_modified, last_write_time(compiled_file_path));
//        BOOST_CHECK_LT(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
//    }
//
//    BOOST_AUTO_TEST_CASE(cached_files_recompiled2) {
//        path tmp = create_tmp();
//        path file_path = tmp / "foo.cutc";
//        path cutc_path = file_path.string() + ".cutc";
//        path compiled_file_path = file_path.string() + ".cutvm";
//        path compiled_cutc_path = cutc_path.string() + ".cutvm";
//        path output_file_path = tmp / "foo.cutvm";
//
//        std::ofstream compiled_cutc_file(compiled_cutc_path.string());
//        compiled_cutc_file << "b s to\n"
//                              "b s .\n"
//                              "b s cutc-tokenizer\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "b s .\n"
//                              "b s cutvm-cache\n"
//                              "b i 1\n"
//                              "c 3 0 array\n"
//                              "c 3 0 array";
//        compiled_cutc_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream cutc_file(cutc_path.string());
//        cutc_file << "'cutc-tokenizer'.1 to 'cutvm-cache'.1";
//        cutc_file.close();
//
//        std::ofstream src_file(file_path.string());
//        src_file << R"(normal_string "'" -> "'")";
//        src_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        std::ofstream compiled_file(compiled_file_path.string());
//        compiled_file << "b s normal_string\n"
//                         "b s ->\n"
//                         "b s '\n"
//                         "b s '\n"
//                         "c 3 0 array\n"
//                         "c 2 0 array";
//        compiled_file.close();
//
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//
//        auto compiled_file_last_modified = last_write_time(compiled_file_path);
//        auto compiled_cutc_last_modified = last_write_time(compiled_cutc_path);
//
//        state.search_path = {tmp};
//        compile_file(state, file_path, compiled_file_path, output_file_path);
//
//        BOOST_CHECK_LT(compiled_file_last_modified, last_write_time(compiled_file_path));
//        BOOST_CHECK_LT(compiled_cutc_last_modified, last_write_time(compiled_cutc_path));
//    }
//
//BOOST_AUTO_TEST_SUITE_END()