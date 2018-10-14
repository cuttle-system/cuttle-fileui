#include "lang_tokenizer.hpp"
#include "lang_parser_cutc.hpp"
#include "fileui_cache.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "translator_methods.hpp"
#include "generator_methods.hpp"
#include "tokenizer_config.hpp"
#include "file_interpreter.hpp"
#include "translator_ui.hpp"
#include "independent_language_config_ui.hpp"
#include "vm_context_methods.hpp"
#include "fileui_file.hpp"
#include "file_not_found_error.hpp"
#include "context_methods.hpp"
#include "std.hpp"

using namespace cuttle;
using namespace cuttle::fileui;

namespace fs = boost::filesystem;

void cache_file(compile_state_t &state, const fs::path &file_path, const fs::path &compiled_file_path,
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

    if (tree.src[tree.src.back()[0]].size() == 2) {
        to.name = tokens[tree.src[tree.src[tree.src.back()[0]][1]][0]].value;
        to.version = std::stoi(tokens[tree.src[tree.src[tree.src.back()[0]][1]][1]].value);
    } else {
        to.name = TRANSLATOR_ANY_NAME;
        to.version = TRANSLATOR_ANY_VERSION;
    }
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

void fileui::get_cached(compile_state_t &state, const fs::path &file_path, const fs::path &cutc_path, fs::path& compiled_file_path,
                language_t &from, language_t &to, call_tree_t &tree, tokens_t &tokens
) {
    if (compiled_file_path.empty()) compiled_file_path = get_compiled_file_path(file_path);

    fs::path compiled_cutc_path = compiled_file_path.parent_path() / fs::path(cutc_path.filename().string() + ".cutvm");

    call_tree_t cutc_tree;
    tokens_t cutc_tokens;

    if (!exists(cutc_path)) {
        throw file_not_found_error(cutc_path);
    }

    if (!exists(compiled_file_path) || !exists(compiled_cutc_path)
        || last_write_time(compiled_file_path) < last_write_time(file_path)
        || last_write_time(compiled_cutc_path) < last_write_time(cutc_path)
    ) {
        context_t cutvm_context;
        tokenizer_config_t cutvm_tokenizer;
        generator_config_t cutvm_generator;
        translator_t cutvm_translator;

        initialize(cutvm_context);

        language_t from_cutc = {TRANSLATOR_ANY_NAME, TRANSLATOR_ANY_VERSION}, to_cutvm = {"cutvm-cache", 1};
        get_independent_language_config(state, to_cutvm,
                cutvm_context, cutvm_tokenizer, cutvm_generator);

        get_language_translator(state, from_cutc, to_cutvm, cutvm_translator);

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
        get_independent_language_config(state, from, context, tokenizer, generator_config);

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
