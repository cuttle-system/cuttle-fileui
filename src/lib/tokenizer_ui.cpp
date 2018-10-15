#include "tokenizer_ui.hpp"
#include "call_tree.hpp"
#include "value.hpp"
#include "lang_tokenizer.hpp"
#include "compiler.hpp"
#include "format_error.hpp"
#include "std.hpp"
#include "vm_context_methods.hpp"
#include "lang_tokenizer_cutvm_functions.hpp"
#include "file_interpreter.hpp"
#include "fileui_file.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void config_init_vm_tokenizer_config(vm::context_t &vm_context,
                                     const std::string &object_var_name, vm::object_t context) {
    vm::populate(vm_context);

    auto parser_context = vm::value_t{{vm::type_id::object},
                                      {context}};
    vm::add(vm_context, object_var_name, parser_context);
    lang::register_lang_tokenizer_cutvm_functions(vm_context);
}

void fileui::interpret_tokenizer_config(const boost::filesystem::path &file_path, tokenizer_config_t &context) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    config_init_vm_tokenizer_config(vm_context, TOKENIZER_CONFIG_VAR_NAME, (vm::object_t) &context);

    interpret_file(vm_context, file_path, arg_stack);
}

void
fileui::get_tokenizer_from_module(compile_state_t &state, const fs::path &module_path, tokenizer_config_t &tokenizer) {
    fs::path tokenizer_file_path = module_path / "parser" / "tokenizer" / "rules.cutl";
    fs::path output_tokenizer_file_path = get_output_file_path(tokenizer_file_path);

    if (fs::exists(tokenizer_file_path)) {
        compile_file(state, tokenizer_file_path);
        interpret_tokenizer_config(output_tokenizer_file_path, tokenizer);
    } else {
        lang::get_tokenizer_config(tokenizer);
    }
    if (tokenizer.macro_p.empty()) tokenizer.macro_p["0p"].insert("0");
    if (tokenizer.macro_pf.empty()) tokenizer.macro_pf["0pf"].insert("0");
    if (tokenizer.macro_ps.empty()) tokenizer.macro_ps["0ps"].insert("0");
    if (tokenizer.macro_if.empty()) tokenizer.macro_if.insert("0if");
}