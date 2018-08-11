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

boost::filesystem::path cuttle::fileui::compile_function(
    const boost::filesystem::path &function_path,
    const boost::filesystem::path &module_path,
    const boost::filesystem::path &compiled_module_path
) {
    using namespace boost::filesystem;

    std::string function_name = function_path.filename().string();
    path compiled_function_path = compiled_module_path / function_name;
    boost::filesystem::create_directory(compiled_function_path);
    // compilation process
    return compiled_function_path;
}

void cuttle::fileui::compile_functions(
    const boost::filesystem::path &module_path,
    const boost::filesystem::path &compiled_module_path
) {
    using namespace boost::filesystem;

    for (const auto& function_path_it : directory_iterator(module_path)) {
        const path& function_path = function_path_it.path();
        cuttle::fileui::compile_function(
            function_path,
            module_path,
            compiled_module_path
        );
    }
}

boost::filesystem::path cuttle::fileui::compile_module(
    const std::string &module_name,
    const std::list<boost::filesystem::path> &search_path
) {
    using namespace cuttle::fileui;

    auto module_path = search_module(module_name, search_path);
    auto compiled_module_path = module_path.parent_path() / "cutvm.compiled" / module_name;
    boost::filesystem::create_directories(compiled_module_path);
    compile_functions(module_path, compiled_module_path);
    return compiled_module_path;
}
