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
#include "dictionary_methods.hpp"
#include "format_error.hpp"
#include "std.hpp"
#include "lang_parser_cutvm_functions.hpp"
#include "lang_parser_cutc_parser.hpp"
#include "file_not_found_error.hpp"
#include "lang_cutvm_translator.hpp"

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

fs::path cuttle::fileui::get_output_module_path(const fs::path &module_path) {
    auto module_name = module_path.filename().string();
    return module_path.parent_path() / (module_name + CUTTLE_FILEUI_OUTPUT_PATH_POSTFIX);
}

fs::path cuttle::fileui::get_output_file_path(const fs::path &file_path) {
    using namespace cuttle::fileui;

    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_output_module_path(module_path);
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

    if (tree.src[tree.src.back()[0]].size() == 2) {
        to.name = tokens[tree.src[tree.src[tree.src.back()[0]][1]][0]].value;
        to.version = std::stoi(tokens[tree.src[tree.src[tree.src.back()[0]][1]][1]].value);
    } else {
        to.name = TRANSLATOR_ANY_NAME;
        to.version = TRANSLATOR_ANY_VERSION;
    }
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

void compile_without_generation(compile_state &state, const path &file_path, path compiled_file_path,
                                call_tree_t &new_tree, values_t &values, language_t &from, language_t &to);

void get_tokenizer_range_from_tree(const path &file_path, tree_src_element_t root_index,
        const call_tree_t &tree, const values_t &values, tokenizer_range_map_t &range_map) {
    auto index = tree.src[root_index][0];
    if (values[index].type == value_type::func_name && values[index].value == "->") {
        range_map[values[tree.src[index][0]].value].insert(values[tree.src[index][1]].value);
    } else {
        throw format_error("expected '->' symbol '" + values[root_index].value + "'", file_path);
    }
}

void get_tokenizer_map_from_tree(const path &file_path, tree_src_element_t root_index,
                                   const call_tree_t &tree, const values_t &values, tokenizer_map_t &tokenizer_map, bool reverse = false) {
    auto index = tree.src[root_index][0];
    if (values[index].type == value_type::func_name && values[index].value == "->") {
        if (reverse) {
            tokenizer_map[values[tree.src[index][1]].value] = values[tree.src[index][0]].value;
        } else {
            tokenizer_map[values[tree.src[index][0]].value] = values[tree.src[index][1]].value;
        }
    } else {
        throw format_error("expected '->' symbol '" + values[root_index].value + "'", file_path);
    }
}

void get_tokenizer_symbol_set_from_tree(const path &file_path, tree_src_element_t root_index,
                                   const call_tree_t &tree, const values_t &values, tokenizer_symbol_set_t &symbol_set) {
    auto index = tree.src[root_index][0];
    symbol_set.insert(values[tree.src[index][0]].value);
}

void get_boolean_from_tree(const path &file_path, tree_src_element_t root_index,
                                        const call_tree_t &tree, const values_t &values, bool &output) {
    auto index = tree.src[root_index][0];
    output = values[tree.src[index][1]].value == "true";
}

void get_tokenizer_from_tree(const path &file_path, const call_tree_t &tree, const values_t &values, tokenizer_config_t &tokenizer) {
    for (auto root_index : tree.src.back()) {
        if (values[root_index].type == value_type::func_name) {
            if (values[root_index].value == "normal_string") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.normal_string);
            } else if (values[root_index].value == "formatted_string") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.formatted_string);
            } else if (values[root_index].value == "comments") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.comments);
            } else if (values[root_index].value == "macro_ps") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.macro_ps);
            } else if (values[root_index].value == "macro_pf") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.macro_pf);
            } else if (values[root_index].value == "macro_p") {
                get_tokenizer_range_from_tree(file_path, root_index, tree, values, tokenizer.macro_p);
            } else if (values[root_index].value == "separated_symbols") {
                get_tokenizer_symbol_set_from_tree(file_path, root_index, tree, values, tokenizer.separated_symbols);
            } else if (values[root_index].value == "macro_if") {
                get_tokenizer_symbol_set_from_tree(file_path, root_index, tree, values, tokenizer.macro_if);
            } else if (values[root_index].value == "formatted_characters") {
                get_tokenizer_map_from_tree(file_path, root_index, tree, values, tokenizer.formatted_characters);
                get_tokenizer_map_from_tree(file_path, root_index, tree, values, tokenizer.formatted_characters_output, true);
            } else if (values[root_index].value == "separate_digit_and_alpha") {
                get_boolean_from_tree(file_path, root_index, tree, values, tokenizer.separate_digit_and_alpha);
            } else {
                throw format_error("undefined property '" + values[root_index].value + "'", file_path);
            }
        } else {
            throw format_error("non-function symbol in root", file_path);
        }
    }
}

void init_vm_context(vm::context_t &vm_context,
        const std::string &array_var_name, vm::value_t array,
        const std::string &object_var_name, vm::object_t context) {
    vm::populate(vm_context);

    auto parser_context = vm::value_t{{vm::type_id::object},
                                      {context}};
    vm::add(vm_context, array_var_name, array);
    vm::add(vm_context, object_var_name, parser_context);
    lang::register_lang_parser_cutvm_functions(vm_context);
}

void interpret_context(const path &file_path, context_t &context) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    auto parser_config_array = PARSER_CONTEXT_CONFIG_ARRAY_DEFAULT_VALUES;
    init_vm_context(vm_context,
            lang::PARSER_CONTEXT_CONFIG_ARRAY_VAR_NAME, parser_config_array,
            lang::PARSER_CONTEXT_VAR_NAME, (vm::object_t) &context);

    std::ifstream config_file(file_path.string());

    while (!config_file.eof()) {
        vm::eval(config_file, vm_context, arg_stack);
    }
    config_file.close();

    vm::value_t ret;
    vm::call(vm_context, "append_to_context", {}, 0, ret);
}

void get_context_from_module(compile_state &state, const path &module_path, context_t &context) {
    path functions_path = module_path / "parser" / "functions";

    if (!exists(functions_path)) {
        return;
    }

    for (const auto &file_path_it : directory_iterator(functions_path)) {
        const path &file_path = file_path_it.path();
        const path &rules_path = file_path / "rules.cutl";
        const path &compiled_file_path = get_compiled_file_path(rules_path);
        const path &output_file_path = get_output_file_path(rules_path);
        call_tree_t context_tree;
        values_t context_values;
        compile_file(state, rules_path);
        interpret_context(output_file_path, context);
    }
}

void get_tokenizer_from_module(compile_state &state, const path &module_path, tokenizer_config_t &tokenizer) {
    language_t from, to;
    call_tree_t tokenizer_tree;
    values_t tokenizer_values;
    path tokenizer_file_path = module_path / "parser" / "tokenizer" / "rules.cutl";

    if (exists(tokenizer_file_path)) {
        compile_without_generation(state, tokenizer_file_path, "",
                                   tokenizer_tree, tokenizer_values, from, to);
        get_tokenizer_from_tree(tokenizer_file_path, tokenizer_tree, tokenizer_values, tokenizer);
    } else {
        lang::get_tokenizer_config(tokenizer);
    }
}

void get_generator_from_module(compile_state &state, const path &module_path, generator_config_t &generator_config) {

}

void get_language_info(compile_state &state, const language_t &lang,
    context_t &context, tokenizer_config_t &tokenizer, generator_config_t &generator_config
) {
    if (lang.name == "cutc-tokenizer" && lang.version == 1) {
        lang::get_parser_cutc_tokenizer(context);
        lang::get_tokenizer_config(tokenizer);
    } else if (lang.name == "cutc-parser" && lang.version == 1) {
        lang::get_parser_cutc_parser(context);
        lang::get_tokenizer_config(tokenizer);
    } else if ((lang.name == "cutvm" || lang.name == "cutvm-cache") && lang.version == 1) {
        lang::get_cutvm_context(context);
        lang::get_cutvm_tokenizer_config(tokenizer);
        lang::get_cutvm_generator_config(generator_config);
    } else {
        auto module_path = search_module(state, lang.name + "." + std::to_string(lang.version));
        get_tokenizer_from_module(state, module_path, tokenizer);
        get_context_from_module(state, module_path, context);
        get_generator_from_module(state, module_path, generator_config);
    }
}

void get_language_translator(compile_state &state, const language_t &from, const language_t &to, translator_t &translator) {
    if (to.name == "cutvm-cache" && to.version == 1) {
        lang::get_cutvm_translator(translator);
    } else if (from.name == "cutc-parser" && from.version == 1 && to.name == "cutvm" && to.version == 1) {
        lang::get_lang_cutvm_translator(translator);
    } else if (to.name == TRANSLATOR_ANY_NAME && to.version == TRANSLATOR_ANY_VERSION) {
        initialize(translator.dictionary);
        translator.to = to;
        translator.from = from;
    } else {
        initialize(translator.dictionary);
        // TODO: search module...
    }
}

void get_cached(compile_state &state, const path &file_path, const path &cutc_path, path& compiled_file_path,
    language_t &from, language_t &to, call_tree_t &tree, tokens_t &tokens
) {
    if (compiled_file_path.empty()) compiled_file_path = get_compiled_file_path(file_path);

    path compiled_cutc_path = compiled_file_path.parent_path() / path(cutc_path.filename().string() + ".cutvm");

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
        get_language_info(state, from, context, tokenizer, generator_config);

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

void compile_without_generation(compile_state &state, const path &file_path, path compiled_file_path,
                                call_tree_t &new_tree, values_t &values, language_t &from, language_t &to) {
    path cutc_path = file_path.string() + ".cutc";

    call_tree_t tree;
    tokens_t tokens;

    if (!exists(file_path)) {
        throw file_not_found_error(file_path);
    }

    get_cached(state, file_path, cutc_path, compiled_file_path, from, to, tree, tokens);

    translator_t translator;
    get_language_translator(state, from, to, translator);
    translate(translator, tokens, tree, values, new_tree);
}

void cuttle::fileui::compile_file(compile_state &state, const path &file_path,
                                  const path &compiled_file_path, path output_file_path) {
    if (output_file_path.empty()) output_file_path = get_output_file_path(file_path);

    values_t values;
    call_tree_t new_tree;
    language_t from, to;

    compile_without_generation(state, file_path, compiled_file_path,
                               new_tree, values,
                               from, to);

    context_t context;
    tokenizer_config_t tokenizer_config;
    generator_config_t generator_config;

    initialize(context);
    if (to.name == TRANSLATOR_ANY_NAME && to.version == TRANSLATOR_ANY_VERSION) {
        get_language_info(state, from, context, tokenizer_config, generator_config);
    } else {
        get_language_info(state, to, context, tokenizer_config, generator_config);
    }
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