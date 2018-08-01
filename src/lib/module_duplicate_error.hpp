#pragma once

#include <stdexcept>

namespace cuttle {
    namespace fileui {
        class module_duplicate_error : public std::logic_error {
        public:
            explicit module_duplicate_error(const std::string& module_name) : logic_error("Module duplicate: " + module_name) {}
        };
    }
}