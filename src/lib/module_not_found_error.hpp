#pragma once

#include <stdexcept>

namespace cuttle {
    namespace fileui {
        class module_not_found_error : public std::logic_error {
        public:
            explicit module_not_found_error(const std::string& module_name) : logic_error("Module not found: " + module_name) {}
        };
    }
}