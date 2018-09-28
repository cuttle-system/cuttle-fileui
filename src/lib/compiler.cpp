#include <string>
#include <boost/filesystem.hpp>
#include "cutvm_context.hpp"
#include "cutvm_generator.hpp"
#include "cutvm_tokenizer.hpp"
#include "generator_methods.hpp"
#include "cutvm_translator.hpp"
#include "translator_methods.hpp"
#include "parser.hpp"
#include "context_methods.hpp"
#include "lang_parser_base.hpp"
#include "lang_parser_cutc.hpp"
#include "compiler.hpp"
#include "module_duplicate_error.hpp"
#include "compile_state.hpp"
#include "incorrect_module_structure_error.hpp"
#include "lang_tokenizer.hpp"
#include "tokenizer.hpp"

namespace fs = boost::filesystem;

fs::path cuttle::fileui::get_parent_module_path(const fs::path &path) {
    using namespace fs;

    auto module_path = path;
    for (; exists(module_path / CUTTLE_FILEUI_ROOT_PATH_FILE); module_path = module_path.parent_path()) {
        if (module_path.parent_path() == "") {
            throw incorrect_module_structure_error(path);
        }
    }

    return module_path;
}

fs::path cuttle::fileui::get_compiled_module_path(const fs::path &module_path) {
    auto module_name = module_path.filename().string();
    return module_path.parent_path() / (module_name + CUTTLE_FILEUI_COMPILED_PATH_POSTFIX);
}

fs::path cuttle::fileui::get_compiled_file_path(const fs::path &file_path) {
    using namespace cuttle::fileui;

    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_compiled_module_path(module_path);
    auto relative_file_path = fs::relative(file_path, module_path);
    return compiled_module_path / relative_file_path;
}

fs::path cuttle::fileui::search_module(const compile_state &state, const std::string &module_name) {
    using namespace fs;

    bool found = false;
    fs::path found_path;
    for (const auto& path : state.search_path) {
        auto expected_path = path / module_name;
        if (fs::exists(expected_path)) {
            if (found) {
                throw cuttle::fileui::module_duplicate_error(module_name);
            }
            found = true;
            found_path = expected_path;
        }
    }
    return found_path;
}

void cuttle::fileui::compile_file(compile_state &state, const boost::filesystem::path &file_path,
                                  boost::filesystem::path compiled_file_path) {
    boost::filesystem::path config_file_path = file_path.string() + ".cutc";

    if (compiled_file_path.empty()) compiled_file_path = get_compiled_file_path(file_path);

    boost::filesystem::path compiled_config_file_path = compiled_file_path.parent_path()
            / boost::filesystem::path(config_file_path.filename().string() + ".cutvm");

    std::ifstream config_file(config_file_path.string());
    std::string config_src((std::istreambuf_iterator<char>(config_file)),
                            std::istreambuf_iterator<char>());
    config_file.close();

    tokenizer_config_t config_tokenizer, cutvm_tokenizer;
    generator_config_t generator_config;
    context_t context, cutvm_context;
    translator_t translator;

    initialize(context);
    initialize(cutvm_context);
    lang::get_tokenizer_config(config_tokenizer);
    lang::get_parser_cutc(context);
    lang::get_cutvm_translator(translator);
    lang::get_cutvm_tokenizer_config(cutvm_tokenizer);
    lang::get_cutvm_generator_config(generator_config);
    lang::get_cutvm_context(cutvm_context);

    tokens_t config_tokens;
    call_tree_t tree, new_tree;
    values_t values;
    generator_state_t generator_state;

    initialize(generator_state);

    tokenize(config_tokenizer, config_src, config_tokens);
    parse(config_tokens, tree, context);
    translate(translator, config_tokens, tree, values, new_tree);
    generate(cutvm_tokenizer, generator_config, cutvm_context, values, new_tree, generator_state);

    std::ofstream compiled_config_file(compiled_config_file_path.string());
    compiled_config_file << generator_state.output;
    compiled_config_file.close();
    // compilation process
}

void cuttle::fileui::compile_files(compile_state &state, const boost::filesystem::path &functions_path) {
    using namespace fs;

    for (const auto& file_path_it : recursive_directory_iterator(functions_path)) {
        const path& file_path = file_path_it.path();
        cuttle::fileui::compile_file(state, file_path);
    }
}