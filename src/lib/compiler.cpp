#include <string>
#include <boost/filesystem.hpp>
#include "compiler.hpp"
#include "file_not_found_error.hpp"
#include "fileui_cache.hpp"
#include "translator_ui.hpp"
#include "translator_methods.hpp"
#include "fileui_file.hpp"
#include "context_methods.hpp"
#include "tokenizer_config.hpp"
#include "generator_config.hpp"
#include "independent_language_config_ui.hpp"
#include "generator_methods.hpp"
#include "fileui_module.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void fileui::compile_without_generation(compile_state_t &state, const fs::path &file_path, fs::path compiled_file_path,
                                call_tree_t &new_tree, values_t &values, language_t &from, language_t &to, bool cache) {
    fs::path cutc_path = file_path.string() + ".cutc";

    call_tree_t tree;
    tokens_t tokens;

    if (!exists(file_path)) {
        throw file_not_found_error(file_path);
    }

    get_cached(state, file_path, cutc_path, compiled_file_path, from, to, tree, tokens, cache);

    translator_t translator;
    get_language_translator(state, from, to, translator);
    translate(translator, tokens, tree, values, new_tree);
}

void fileui::compile_file(compile_state_t &state, const fs::path &file_path,
                                  const fs::path &compiled_file_path, fs::path output_file_path, bool cache) {
    if (output_file_path.empty()) {
        output_file_path = get_output_file_path(file_path);
        create_directories(output_file_path.parent_path());
        std::ofstream cutroot_file(
                (get_output_module_path(get_parent_module_path(file_path)) / CUTTLE_FILEUI_ROOT_PATH_FILE).string());
        cutroot_file << "";
        cutroot_file.close();
    }

    values_t values;
    call_tree_t new_tree;
    language_t from, to;

    compile_without_generation(state, file_path, compiled_file_path,
                               new_tree, values,
                               from, to, cache);

    context_t context;
    tokenizer_config_t tokenizer_config;
    generator_config_t generator_config;

    initialize(context);

    get_independent_language_config(state, to, context, tokenizer_config, generator_config);

    generator_state_t generator_state;
    generate(tokenizer_config, generator_config, context, values, new_tree, generator_state);

    std::ofstream output_file(output_file_path.string());
    output_file << generator_state.output;
    output_file.close();
}

void fileui::compile_files(compile_state_t &state, const fs::path &functions_path) {
    for (const auto& file_path_it : fs::recursive_directory_iterator(functions_path)) {
        const fs::path& file_path = file_path_it.path();
        compile_file(state, file_path);
    }
}