#pragma once

#include <deque>
#include <boost/filesystem.hpp>
#include "compile_state.hpp"
#include "vm_context.hpp"

namespace cuttle {
    namespace fileui {
        void interpret_file(compile_state_t &state, vm::context_t &context, const boost::filesystem::path &file_path,
                            std::deque<vm::value_t> &arg_stack);
    }
}