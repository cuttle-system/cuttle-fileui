#include "config_vm_interface.hpp"
#include "std.hpp"
#include "vm_context_methods.hpp"
#include "lang_parser_cutvm_functions.hpp"

using namespace cuttle;

void fileui::config_init_vm_context(vm::context_t &vm_context,
                            const std::string &array_var_name, vm::value_t array,
                            const std::string &object_var_name, vm::object_t context) {
    vm::populate(vm_context);

    auto parser_context = vm::value_t{{vm::type_id::object},
                                      {context}};
    vm::add(vm_context, array_var_name, array);
    vm::add(vm_context, object_var_name, parser_context);
    lang::register_lang_parser_cutvm_functions(vm_context);
}