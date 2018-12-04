#include <iostream>
#include "context.hpp"
#include "generator_config.hpp"
#include "translator_ui.hpp"
#include "cutvm_translator.hpp"
#include "lang_cutvm_translator.hpp"
#include "dictionary_methods.hpp"
#include "fileui_module.hpp"
#include "fileui_file.hpp"
#include "file_not_found_error.hpp"
#include "compiler.hpp"
#include "tokenizer_config.hpp"
#include "fileui_cache.hpp"
#include "independent_language_config_ui.hpp"
#include "dictionary_funcs.hpp"
#include "lang_output_translator.hpp"

using namespace cuttle;
namespace fs = boost::filesystem;

void set_translator_output_tree(fileui::compile_state_t &state, language_t lang, const fs::path &file_path, std::string &vm_src) {
    const fs::path &output_file_path = fileui::get_output_file_path(file_path);
    if (state.cached_files.count(output_file_path.string())) {
        vm_src = state.cached_files[output_file_path.string()];
    } else {
        fileui::compile_file(state, file_path);
        std::ifstream file (output_file_path.string());

        vm_src = std::string((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        state.cached_files[output_file_path.string()] = vm_src;
    }
}

void
get_translator_function_tree(fileui::compile_state_t &state, language_t lang, const fs::path &file_path,
                             call_tree_t &tree, tokens_t &tokens) {
    const fs::path &output_file_path = fileui::get_output_file_path(file_path);
    const fs::path &cutc_path = output_file_path.string() + ".cutc";
    fs::path compiled_output_file_path = fileui::get_compiled_file_path(output_file_path);

    fileui::compile_file(state, file_path);

    std::ofstream cutc_file(cutc_path.string());
    cutc_file << "just '" + lang.name + "'." + std::to_string(lang.version);
    cutc_file.close();

    language_t to = lang;

    fileui::get_cached(state, output_file_path, cutc_path, compiled_output_file_path, lang, to, tree, tokens);
}

void
get_language_translator_from_module(fileui::compile_state_t &state, const fs::path &module_path, const language_t &from,
                                    const language_t &to, translator_t &translator) {
    std::string translator_name = to.name + "." + std::to_string(to.version);
    fs::path translator_path = module_path / "translators" / translator_name;

    if (!exists(translator_path)) {
        return;
    }

    for (const auto &function_path_it : fs::directory_iterator(translator_path / "functions")) {
        const fs::path &function_path = function_path_it.path();
        const fs::path &pattern_path = fileui::find_file(function_path / "pattern");
        const fs::path &output_path = fileui::find_file(function_path / "output");
        std::string vm_src;
        call_tree_t pattern_tree, output_tree;
        tokens_t pattern_tokens, output_tokens;
        get_translator_function_tree(state, from, pattern_path, pattern_tree, pattern_tokens);
        set_translator_output_tree(state, to, output_path, vm_src);
        auto func_id = add(translator.dictionary, pattern_tree, pattern_tokens, dictionary_funcs::apply_pattern_output);
        translator.dictionary.vm_output_trees.insert({func_id, vm_src});
    }
}

void fileui::get_language_translator(compile_state_t &state, const language_t &from, const language_t &to,
                                     translator_t &translator) {
    if (to.name == "cutvm-cache" && to.version == 1) {
        lang::get_cutvm_translator(translator);
    } else if (to.name == "cutvm-translator-output" && to.version == 1) {
        lang::get_output_translator(translator);
    } else if ((
                       (from.name == "cutc-tokenizer" && from.version == 1) ||
                       (from.name == "cutc-parser" && from.version == 1) ||
                       (from.name == "cutc-generator-func" && from.version == 1) ||
                       (from.name == "cutc-generator-arg" && from.version == 1)
               ) && to.name == "cutvm" && to.version == 1) {
        lang::get_lang_cutvm_translator(translator);
    } else if (to.name == from.name & to.version == from.version) {
        initialize(translator.dictionary);
        translator.to = to;
        translator.from = from;
    } else {
        initialize(translator.dictionary);
        auto module_path = search_module(state, from.name + "." + std::to_string(from.version));
        get_language_translator_from_module(state, module_path, from, to, translator);
    }
}
