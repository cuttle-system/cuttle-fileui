#include "fileui_file.hpp"
#include "fileui_module.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

fs::path fileui::get_compiled_file_path(const fs::path &file_path) {
    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_compiled_module_path(module_path);
    auto relative_file_path = fs::relative(file_path, module_path);
    return compiled_module_path / relative_file_path;
}

fs::path cuttle::fileui::get_output_file_path(const fs::path &file_path) {
    using namespace cuttle::fileui;

    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_output_module_path(module_path);
    auto relative_file_path = fs::relative(file_path, module_path);
    return compiled_module_path / relative_file_path;
}