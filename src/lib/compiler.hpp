#pragma once

#include <boost/filesystem.hpp>
#include "compile_state.hpp"
#include "call_tree.hpp"
#include "value.hpp"

namespace cuttle {
    namespace fileui {
        void compile_without_generation(compile_state_t &state, const boost::filesystem::path &file_path,
                                        boost::filesystem::path compiled_file_path,
                                        call_tree_t &new_tree, values_t &values, language_t &from, language_t &to, bool cache = true);

        void compile_file(compile_state_t &state, const boost::filesystem::path &file_path,
                          const boost::filesystem::path &compiled_file_path = "",
                          boost::filesystem::path output_file_path = "", bool cache = true);

        void compile_files(compile_state_t &state, const boost::filesystem::path &module_path);
    }
}
