#include "fileui_module.hpp"
#include "incorrect_module_structure_error.hpp"
#include "module_duplicate_error.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

fs::path fileui::get_parent_module_path(const fs::path &path) {
    auto module_path = path;
    for (; fs::exists(module_path / CUTTLE_FILEUI_ROOT_PATH_FILE); module_path = module_path.parent_path()) {
        if (module_path.parent_path() == "") {
            throw incorrect_module_structure_error(path);
        }
    }

    return module_path;
}

fs::path fileui::get_compiled_module_path(const fs::path &module_path) {
    auto module_name = module_path.filename().string();
    return module_path.parent_path() / (module_name + CUTTLE_FILEUI_COMPILED_PATH_POSTFIX);
}

fs::path fileui::get_output_module_path(const fs::path &module_path) {
    auto module_name = module_path.filename().string();
    return module_path.parent_path() / (module_name + CUTTLE_FILEUI_OUTPUT_PATH_POSTFIX);
}

fs::path fileui::search_module(const compile_state &state, const std::string &module_name) {
    bool found = false;
    fs::path found_path;
    for (const auto& path : state.search_path) {
        auto expected_path = path / module_name;
        if (fs::exists(expected_path)) {
            if (found) {
                throw module_duplicate_error(module_name);
            }
            found = true;
            found_path = expected_path;
        }
    }
    return found_path;
}