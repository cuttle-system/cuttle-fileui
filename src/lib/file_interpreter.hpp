#pragma once

#include <deque>
#include <boost/filesystem.hpp>
#include "vm_context.hpp"

namespace cuttle {
    namespace fileui {
        void interpret_file(vm::context_t &context, const boost::filesystem::path &file_path,
                            std::deque<vm::value_t> &arg_stack);
    }
}