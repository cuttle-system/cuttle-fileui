#include <string>
#include <boost/filesystem.hpp>
#include "module_compiler.hpp"
#include "module_duplicate_error.hpp"

boost::filesystem::path cuttle::fileui::search_module(const std::string& module_name, const std::list<boost::filesystem::path> &search_path) {
    using namespace boost::filesystem;

    bool found = false;
    boost::filesystem::path found_path;
    for (const auto& path : search_path) {
        auto expected_path = path / module_name;
        if (boost::filesystem::exists(expected_path)) {
            if (found) {
                throw cuttle::fileui::module_duplicate_error(module_name);
            }
            found = true;
            found_path = expected_path;
        }
    }
    return found_path;
}

void cuttle::fileui::compile_module(const std::string &module_name,
                                    const std::list<boost::filesystem::path> &search_path) {
    using namespace cuttle::fileui;

    auto module_path = search_module(module_name, search_path);
}