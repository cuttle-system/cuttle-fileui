#include "parser_ui.hpp"
#include "interpreter.hpp"
#include "lang_parser_cutvm_functions.hpp"
#include "vm_context_methods.hpp"
#include "fileui_file.hpp"
#include "call_tree.hpp"
#include "value.hpp"
#include "compiler.hpp"
#include "std.hpp"
#include "file_interpreter.hpp"

using namespace cuttle;
namespace fs = boost::filesystem;

void config_init_vm_context(vm::context_t &vm_context,
                            const std::string &array_var_name, vm::value_t array,
                            const std::string &object_var_name, vm::object_t context) {
    vm::populate(vm_context);

    auto parser_context = vm::value_t{{vm::type_id::object},
                                      {context}};
    vm::add(vm_context, array_var_name, array);
    vm::add(vm_context, object_var_name, parser_context);
    lang::register_lang_parser_cutvm_functions(vm_context);
}

void fileui::interpret_context(fileui::compile_state_t &state, const fs::path &file_path, context_t &context) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    auto parser_config_array = PARSER_CONTEXT_CONFIG_ARRAY_DEFAULT_VALUES;
    config_init_vm_context(vm_context,
                           PARSER_CONTEXT_ARRAY_VAR_NAME, parser_config_array,
                           PARSER_CONTEXT_VAR_NAME, (vm::object_t) &context);

    interpret_file(state, vm_context, file_path, arg_stack);

    vm::value_t ret;
    vm::call(vm_context, "append_to_context", {}, 0, ret);
}

void fileui::get_context_from_module(fileui::compile_state_t &state, const fs::path &module_path, context_t &context) {
    fs::path functions_path = module_path / "parser" / "functions";

    if (!exists(functions_path)) {
        return;
    }

    std::vector<fs::path> files;
    std::copy(fs::directory_iterator(functions_path), fs::directory_iterator(), back_inserter(files));
    sort(files.begin(), files.end());

    for (const fs::path &file_path : files) {
        const fs::path &rules_path = file_path / "rules.cutl";
        const fs::path &compiled_file_path = fileui::get_compiled_file_path(rules_path);
        const fs::path &output_file_path = fileui::get_output_file_path(rules_path);
        call_tree_t context_tree;
        values_t context_values;
        compile_file(state, rules_path);
        try {
            interpret_context(state, output_file_path, context);
        } catch (std::exception &exc) {
            throw std::runtime_error(exc.what() + std::string(" in ") + file_path.string());
        }
    }
}
