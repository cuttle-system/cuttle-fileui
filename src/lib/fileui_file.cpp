#include "fileui_file.hpp"
#include "fileui_module.hpp"
#include "file_not_found_error.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

fs::path fileui::get_compiled_file_path(const fs::path &file_path) {
    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_compiled_module_path(module_path);
    auto relative_file_path = fs::relative(file_path, module_path).string() + ".cutvm";
    return compiled_module_path / relative_file_path;
}

fs::path fileui::get_output_file_path(const fs::path &file_path) {
    auto module_path = get_parent_module_path(file_path);
    auto compiled_module_path = get_output_module_path(module_path);
    auto relative_file_path = fs::relative(file_path, module_path).string();
    return compiled_module_path / relative_file_path;
}

fs::path fileui::find_file(const fs::path &file_path_without_extension) {
    fs::path directory_path = file_path_without_extension.parent_path();
    std::string filename = file_path_without_extension.filename().string();

    for (const auto& file_path_it : fs::directory_iterator(directory_path)) {
        const fs::path& file_path = file_path_it.path();
        if (file_path.stem().string() == filename) {
            return file_path;
        }
    }

    throw file_not_found_error(file_path_without_extension);
}
