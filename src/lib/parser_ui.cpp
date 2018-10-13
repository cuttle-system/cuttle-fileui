#include "parser_ui.hpp"
#include "config_vm_interface.hpp"
#include "interpreter.hpp"
#include "lang_parser_cutvm_functions.hpp"
#include "vm_context_methods.hpp"
#include "fileui_file.hpp"
#include "call_tree.hpp"
#include "value.hpp"
#include "compiler.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void fileui::interpret_context(const fs::path &file_path, context_t &context) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    auto parser_config_array = PARSER_CONTEXT_CONFIG_ARRAY_DEFAULT_VALUES;
    fileui::config_init_vm_context(vm_context,
                    PARSER_CONTEXT_ARRAY_VAR_NAME, parser_config_array,
                    PARSER_CONTEXT_VAR_NAME, (vm::object_t) &context);

    std::ifstream config_file(file_path.string());

    while (!config_file.eof()) {
        vm::eval(config_file, vm_context, arg_stack);
    }
    config_file.close();

    vm::value_t ret;
    vm::call(vm_context, "append_to_context", {}, 0, ret);
}

void fileui::get_context_from_module(fileui::compile_state &state, const fs::path &module_path, context_t &context) {
    fs::path functions_path = module_path / "parser" / "functions";

    if (!exists(functions_path)) {
        return;
    }

    for (const auto &file_path_it : fs::directory_iterator(functions_path)) {
        const fs::path &file_path = file_path_it.path();
        const fs::path &rules_path = file_path / "rules.cutl";
        const fs::path &compiled_file_path = fileui::get_compiled_file_path(rules_path);
        const fs::path &output_file_path = fileui::get_output_file_path(rules_path);
        call_tree_t context_tree;
        values_t context_values;
        compile_file(state, rules_path);
        interpret_context(output_file_path, context);
    }
}
