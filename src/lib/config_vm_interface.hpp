#pragma once

#include <string>
#include "vm_context.hpp"
#include "vm_value.hpp"

namespace cuttle {
    namespace fileui {
        void config_init_vm_context(vm::context_t &vm_context,
                                    const std::string &array_var_name, vm::value_t array,
                                    const std::string &object_var_name, vm::object_t context);
    }
}