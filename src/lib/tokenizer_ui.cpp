#include "tokenizer_ui.hpp"
#include "call_tree.hpp"
#include "value.hpp"
#include "lang_tokenizer.hpp"
#include "compiler.hpp"
#include "format_error.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void get_tokenizer_range_from_tree(const fs::path &file_path, tree_src_element_t root_index,
                                   const call_tree_t &tree, const values_t &values, tokenizer_range_map_t &range_map) {
    using namespace fileui;

    auto index = tree.src[root_index][0];
    if (values[index].type == value_type::func_name && values[index].value == "->") {
        range_map[values[tree.src[index][0]].value].insert(values[tree.src[index][1]].value);
    } else {
        throw format_error("expected '->' symbol '" + values[root_index].value + "'", file_path);
    }
}

void get_tokenizer_map_from_tree(const fs::path &file_path, tree_src_element_t root_index,
                                 const call_tree_t &tree, const values_t &values, tokenizer_map_t &tokenizer_map,
                                 bool reverse = false) {
    using namespace fileui;

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

void get_tokenizer_symbol_set_from_tree(const fs::path &file_path, tree_src_element_t root_index,
                                        const call_tree_t &tree, const values_t &values,
                                        tokenizer_symbol_set_t &symbol_set) {
    auto index = tree.src[root_index][0];
    symbol_set.insert(values[tree.src[index][0]].value);
}

void get_boolean_from_tree(const fs::path &file_path, tree_src_element_t root_index,
                           const call_tree_t &tree, const values_t &values, bool &output) {
    auto index = tree.src[root_index][0];
    output = values[tree.src[index][1]].value == "true";
}

void get_tokenizer_from_tree(const fs::path &file_path, const call_tree_t &tree, const values_t &values,
                             tokenizer_config_t &tokenizer) {
    using namespace fileui;

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
                get_tokenizer_map_from_tree(file_path, root_index, tree, values, tokenizer.formatted_characters_output,
                                            true);
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

void
fileui::get_tokenizer_from_module(compile_state &state, const fs::path &module_path, tokenizer_config_t &tokenizer) {
    language_t from, to;
    call_tree_t tokenizer_tree;
    values_t tokenizer_values;
    fs::path tokenizer_file_path = module_path / "parser" / "tokenizer" / "rules.cutl";

    if (fs::exists(tokenizer_file_path)) {
        compile_without_generation(state, tokenizer_file_path, "",
                                   tokenizer_tree, tokenizer_values, from, to);
        get_tokenizer_from_tree(tokenizer_file_path, tokenizer_tree, tokenizer_values, tokenizer);
    } else {
        lang::get_tokenizer_config(tokenizer);
    }
}