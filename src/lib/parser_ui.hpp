#pragma once

#include <boost/filesystem.hpp>
#include "context.hpp"
#include "compile_state.hpp"

namespace cuttle {
    namespace fileui {
        void interpret_context(compile_state_t &state, const boost::filesystem::path &file_path, context_t &context);

        void
        get_context_from_module(compile_state_t &state, const boost::filesystem::path &module_path, context_t &context);
    }
}