#pragma once

#include <boost/filesystem.hpp>
#include "compile_state.hpp"
#include "call_tree.hpp"
#include "token.hpp"

namespace cuttle {
    namespace fileui {
        void get_cached(compile_state_t &state, const boost::filesystem::path &file_path,
                        const boost::filesystem::path &cutc_path, boost::filesystem::path &compiled_file_path,
                        language_t &from, language_t &to, call_tree_t &tree, tokens_t &tokens, bool cache = true);
    }
}