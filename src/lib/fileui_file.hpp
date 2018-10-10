#pragma once

#include <boost/filesystem.hpp>

namespace cuttle {
    namespace fileui {
        boost::filesystem::path get_compiled_file_path(const boost::filesystem::path &function_path);
        boost::filesystem::path get_output_file_path(const boost::filesystem::path &function_path);
    }
}