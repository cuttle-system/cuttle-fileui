#include <string>
#include <boost/filesystem.hpp>
#include "vm_value.hpp"
#include "interpreter.hpp"
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
#include "lang_parser_cutc_tokenizer.hpp"
#include "compiler.hpp"
#include "module_duplicate_error.hpp"
#include "compile_state.hpp"
#include "incorrect_module_structure_error.hpp"
#include "lang_tokenizer.hpp"
#include "tokenizer.hpp"
#include "vm_context_methods.hpp"
#include "std.hpp"

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

using namespace boost::filesystem;
using namespace cuttle::fileui;
using namespace cuttle;

void cache_file(compile_state &state, const path &file_path, const path &compiled_file_path,
    context_t &context, const tokenizer_config_t &tokenizer,
    context_t &cutvm_context, const tokenizer_config_t &cutvm_tokenizer,
    translator_t &cutvm_translator, generator_config_t &cutvm_generator,
    call_tree_t &tree, tokens_t &tokens,
    call_tree_t &new_tree, values_t &values
) {
    std::ifstream config_file(file_path.string());
    std::string src((std::istreambuf_iterator<char>(config_file)),
                           std::istreambuf_iterator<char>());
    config_file.close();

    generator_state_t generator_state;

    initialize(generator_state);

    tokenize(tokenizer, src, tokens);
    parse(tokens, tree, context);
    translate(cutvm_translator, tokens, tree, values, new_tree);
    generate(cutvm_tokenizer, cutvm_generator, cutvm_context, values, new_tree, generator_state);

    std::ofstream compiled_config_file(compiled_file_path.string());
    compiled_config_file << generator_state.output;
    compiled_config_file.close();
}

void get_languages_config(const call_tree_t &tree, const tokens_t &tokens, language_t &from, language_t &to) {
    from.name = tokens[tree.src[tree.src[tree.src.back()[0]][0]][0]].value;
    from.version = std::stoi(tokens[tree.src[tree.src[tree.src.back()[0]][0]][1]].value);

    to.name = tokens[tree.src[tree.src[tree.src.back()[0]][1]][0]].value;
    to.version = std::stoi(tokens[tree.src[tree.src[tree.src.back()[0]][1]][1]].value);
}

void interpret_file(vm::context_t &context, const path &file_path, std::deque<vm::value_t> &arg_stack) {
    std::ifstream config_file(file_path.string());
    while (!config_file.eof()) {
        vm::eval(config_file, context, arg_stack);
    }
    config_file.close();
}

token_t token_from_vm_value(vm::context_t &context, const vm::value_t &value) {
    vm::value_t ret;
    switch (value.type.id) {
        case vm::type_id::string:
            return token_t{token_type::string, *value.data.string};
        case vm::type_id::integral:
            vm::call(context, "string", {value}, 1, ret);
            return token_t{token_type::number, *ret.data.string};
        default:
            throw std::invalid_argument("A token can't be constructed from the value");
    }
}

tree_src_element_t construct_tree_inner(vm::context_t &context, const std::vector<vm::value_t> &call_array, call_tree_t &tree, tokens_t &tokens) {
    const std::string &function_name = *call_array[0].data.string;
    tree_src_element_t function_index = (tree_src_element_t) tokens.size(), child_index;
    tokens.push_back({token_type::atom, function_name});
    tree.src.push_back({});

    for (unsigned i = 1; i < call_array.size(); ++i) {
        if (call_array[i].type.id == vm::type_id::array) {
            child_index = construct_tree_inner(context, *call_array[i].data.array, tree, tokens);
        } else {
            child_index = (tree_src_element_t) tokens.size();
            tree.src.push_back({});
            tokens.push_back(token_from_vm_value(context, call_array[i]));
        }
        tree.src[function_index].push_back(child_index);
    }

    return function_index;
}

void construct_tree(vm::context_t &context, std::deque<vm::value_t> &arg_stack, call_tree_t &tree, tokens_t &tokens) {
    const std::vector<vm::value_t> &call_array = *arg_stack.begin()->data.array;
    auto i = construct_tree_inner(context, call_array, tree, tokens);
    tree.src.push_back({i});
}

void get_language_info(const language_t &lang,
    context_t &context, tokenizer_config_t &tokenizer, generator_config_t &generator_config
) {
    if (lang.name == "cutc-tokenizer" && lang.version == 1) {
        lang::get_parser_cutc_tokenizer(context);
        lang::get_tokenizer_config(tokenizer);
    } else if (lang.name == "cutvm" && lang.version == 1) {
        lang::get_cutvm_context(context);
        lang::get_cutvm_tokenizer_config(tokenizer);
        lang::get_cutvm_generator_config(generator_config);
    } else {
        // TODO: search module...
    }
}

void get_language_translator(const language_t &from, const language_t &to, translator_t &translator) {
    if (to.name == "cutvm" && to.version == 1) {
        lang::get_cutvm_translator(translator);
    } else {
        // TODO: search module...
    }
}

void get_cached(compile_state &state, const path &file_path, const path &cutc_path, path compiled_file_path,
    language_t &from, language_t &to, call_tree_t &tree, tokens_t &tokens
) {
    if (compiled_file_path.empty()) compiled_file_path = get_compiled_file_path(file_path);

    path compiled_cutc_path = compiled_file_path.parent_path() / path(cutc_path.filename().string() + ".cutvm");

    call_tree_t cutc_tree;
    tokens_t cutc_tokens;

    if (!exists(compiled_file_path) || !exists(compiled_cutc_path)
        || last_write_time(compiled_file_path) < last_write_time(file_path)
        || last_write_time(compiled_cutc_path) < last_write_time(cutc_path)
    ) {
        context_t cutvm_context;
        tokenizer_config_t cutvm_tokenizer;
        generator_config_t cutvm_generator;
        translator_t cutvm_translator;

        initialize(cutvm_context);
        lang::get_cutvm_context(cutvm_context);
        lang::get_cutvm_generator_config(cutvm_generator);
        lang::get_cutvm_tokenizer_config(cutvm_tokenizer);
        lang::get_cutvm_translator(cutvm_translator);

        context_t cutc_context;
        tokenizer_config_t cutc_tokenizer;
        initialize(cutc_context);
        lang::get_parser_cutc(cutc_context);
        lang::get_tokenizer_config(cutc_tokenizer);

        call_tree_t cutc_compiled_tree;
        values_t cutc_compiled_values;

        cache_file(state, cutc_path, compiled_cutc_path, cutc_context, cutc_tokenizer,
                cutvm_context, cutvm_tokenizer, cutvm_translator, cutvm_generator,
                cutc_tree, cutc_tokens, cutc_compiled_tree, cutc_compiled_values);

        context_t context;
        tokenizer_config_t tokenizer;
        generator_config_t generator_config;
        initialize(context);

        get_languages_config(cutc_tree, cutc_tokens, from, to);
        get_language_info(from, context, tokenizer, generator_config);

        call_tree_t file_compiled_tree;
        values_t file_compiled_values;

        cache_file(state, file_path, compiled_file_path, context, tokenizer,
                cutvm_context, cutvm_tokenizer, cutvm_translator, cutvm_generator,
                tree, tokens, file_compiled_tree, file_compiled_values);
    } else {
        std::deque<vm::value_t> cutc_stack, stack;
        vm::context_t vm_context1, vm_context2;

        vm::populate(vm_context1);
        vm::populate(vm_context2);

        interpret_file(vm_context1, compiled_cutc_path, cutc_stack);
        interpret_file(vm_context2, compiled_file_path, stack);

        construct_tree(vm_context1, cutc_stack, cutc_tree, cutc_tokens);
        construct_tree(vm_context2, stack, tree, tokens);

        get_languages_config(cutc_tree, cutc_tokens, from, to);
    }
}

void cuttle::fileui::compile_file(compile_state &state, const boost::filesystem::path &file_path,
                                  const boost::filesystem::path &compiled_file_path, const boost::filesystem::path &output_file_path) {
    path cutc_path = file_path.string() + ".cutc";

    call_tree_t tree;
    tokens_t tokens;

    language_t from, to;

    get_cached(state, file_path, cutc_path, compiled_file_path, from, to, tree, tokens);

    context_t context;
    tokenizer_config_t tokenizer_config;
    generator_config_t generator_config;
    translator_t translator;

    initialize(context);
    get_language_info(to, context, tokenizer_config, generator_config);
    get_language_translator(from, to, translator);

    values_t values;
    call_tree_t new_tree;
    translate(translator, tokens, tree, values, new_tree);

    generator_state_t generator_state;
    generate(tokenizer_config, generator_config, context, values, new_tree, generator_state);

    std::ofstream output_file(output_file_path.string());
    output_file << generator_state.output;
    output_file.close();
}

void cuttle::fileui::compile_files(compile_state &state, const boost::filesystem::path &functions_path) {
    using namespace fs;

    for (const auto& file_path_it : recursive_directory_iterator(functions_path)) {
        const path& file_path = file_path_it.path();
        cuttle::fileui::compile_file(state, file_path);
    }
}