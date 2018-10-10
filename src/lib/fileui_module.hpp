#pragma once

#include <boost/filesystem.hpp>
#include "compile_state.hpp"

#define CUTTLE_FILEUI_ROOT_PATH_FILE ".cutroot"
#define CUTTLE_FILEUI_COMPILED_PATH_POSTFIX ".cutvm.cached"
#define CUTTLE_FILEUI_OUTPUT_PATH_POSTFIX ".cutvm.output"

namespace cuttle {
    namespace fileui {
        boost::filesystem::path get_parent_module_path(const boost::filesystem::path &path);
        boost::filesystem::path get_compiled_module_path(const boost::filesystem::path &module_path);

        boost::filesystem::path get_output_module_path(const boost::filesystem::path &module_path);

        boost::filesystem::path search_module(
                const compile_state &state,
                const std::string &module_name);
    }
}