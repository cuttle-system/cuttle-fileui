#include "generator_ui.hpp"
#include "interpreter.hpp"
#include "lang_generator_func_cutvm_functions.hpp"
#include "lang_generator_arg_cutvm_functions.hpp"
#include "vm_context_methods.hpp"
#include "fileui_file.hpp"
#include "call_tree.hpp"
#include "value.hpp"
#include "compiler.hpp"
#include "std.hpp"
#include "file_interpreter.hpp"

using namespace cuttle;
namespace fs = boost::filesystem;

void config_init_vm_generator_config_func(vm::context_t &vm_context,
                                          const std::string &array_var_name, vm::value_t array,
                                          const std::string &object_var_name, vm::object_t context) {
    vm::populate(vm_context);

    auto generator_config = vm::value_t{{vm::type_id::object},
                                        {context}};
    vm::add(vm_context, array_var_name, array);
    vm::add(vm_context, object_var_name, generator_config);
    lang::register_lang_generator_func_cutvm_functions(vm_context);
}

void config_init_vm_generator_config_args(vm::context_t &vm_context,
        const std::string &object_var_name, vm::object_t context,
        const std::string &argi_var_name, vm::integral_t argi) {
    vm::populate(vm_context);

    auto presenter_params = vm::value_t{{vm::type_id::object},
                                        {context}};
    vm::add(vm_context, object_var_name, presenter_params);

    auto argi_value = vm::value_t{{vm::type_id::integral},
                                        {new vm::integral_t {argi}}};
    vm::add(vm_context, object_var_name, presenter_params);
    vm::add(vm_context, argi_var_name, argi_value);
    lang::register_lang_generator_arg_cutvm_functions(vm_context);
}

std::string interpret_generator_config(const fs::path &file_path, generator_config_t &generator_config) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    auto generator_config_array = GENERATOR_CONFIG_ARRAY_DEFAULT_VALUES;
    config_init_vm_generator_config_func(vm_context,
                                         GENERATOR_CONFIG_ARRAY_VAR_NAME, generator_config_array,
                                         GENERATOR_CONFIG_VAR_NAME, (vm::object_t) &generator_config);

    fileui::interpret_file(vm_context, file_path, arg_stack);

    vm::value_t ret;
    vm::call(vm_context, "append_to_generator_config", {}, 0, ret);
    return *arg_stack.back().data.string;
}

std::string
interpret_generator_config_args(const fs::path &file_path, generator_presenter_params_t &presenter_params, int argi) {
    std::deque<vm::value_t> arg_stack;
    vm::context_t vm_context;

    config_init_vm_generator_config_args(vm_context,
                                         PRESENTER_PARAMS_VAR_NAME, (vm::object_t) &presenter_params,
                                         PRESENTER_PARAMS_ARGI_VAR_NAME, argi);

    fileui::interpret_file(vm_context, file_path, arg_stack);

    vm::value_t ret;
    return *arg_stack.back().data.string;
}

void get_generator_args(fileui::compile_state_t &state, const fs::path &args_path,
                        generator_presenter_params_t &presenter_params) {
    for (const auto &file_path_it : fs::directory_iterator(args_path)) {
        const fs::path &file_path = file_path_it.path();
        const fs::path &rules_path = file_path / "rules.cutl";
        const fs::path &output_file_path = fileui::get_output_file_path(rules_path);
        compile_file(state, rules_path);
        interpret_generator_config_args(output_file_path, presenter_params, std::stoi(file_path.filename().string()));
    }
}

void fileui::get_generator_from_module(compile_state_t &state, const fs::path &module_path,
                                       generator_config_t &generator_config) {
    fs::path functions_path = module_path / "generator" / "functions";

    if (!exists(functions_path)) {
        return;
    }

    for (const auto &file_path_it : fs::directory_iterator(functions_path)) {
        const fs::path &file_path = file_path_it.path();
        const fs::path &rules_path = file_path / "rules.cutl";
        const fs::path &args_path = file_path / "args";
        const fs::path &output_file_path = fileui::get_output_file_path(rules_path);
        call_tree_t context_tree;
        values_t context_values;
        compile_file(state, rules_path);
        auto function_name = interpret_generator_config(output_file_path, generator_config);
        if (exists(args_path)) {
            get_generator_args(state, args_path, generator_config.presenters_params[function_name]);
        }
    }
}