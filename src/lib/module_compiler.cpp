#include <string>
#include <boost/filesystem.hpp>
#include "module_compiler.hpp"
#include "module_duplicate_error.hpp"
#include "compile_state.hpp"

boost::filesystem::path cuttle::fileui::get_compiled_module_path(const boost::filesystem::path &module_path) {
    auto module_name = module_path.filename();
    return module_path.parent_path() / COMPILED_PATH_PREFIX / module_name;
}

boost::filesystem::path cuttle::fileui::get_compiled_function_path(const boost::filesystem::path &function_path) {
    using namespace cuttle::fileui;
    auto function_name = function_path.filename();
    auto compiled_module_path = get_compiled_module_path(function_path.parent_path());
    return compiled_module_path / function_name;
}

boost::filesystem::path cuttle::fileui::search_module(
    const std::string& module_name,
    const cuttle::fileui::compile_state& state
) {
    using namespace boost::filesystem;

    bool found = false;
    boost::filesystem::path found_path;
    for (const auto& path : state.search_path) {
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

void cuttle::fileui::compile_function(
    const boost::filesystem::path &function_path,
    const boost::filesystem::path &module_path,
    cuttle::fileui::compile_state& state
) {
    using namespace boost::filesystem;

    auto function_name = function_path.filename();
    path compiled_function_path = get_compiled_function_path(function_path);
    boost::filesystem::create_directories(compiled_function_path);
    // compilation process
}

void cuttle::fileui::compile_functions(
    const boost::filesystem::path &module_path,
    cuttle::fileui::compile_state& state
) {
    using namespace boost::filesystem;

    for (const auto& function_path_it : directory_iterator(module_path)) {
        const path& function_path = function_path_it.path();
        cuttle::fileui::compile_function(
            function_path,
            module_path,
            state
        );
    }
}

boost::filesystem::path cuttle::fileui::compile_module(
    const std::string &module_name,
    cuttle::fileui::compile_state& state
) {
    using namespace cuttle::fileui;

    auto module_path = search_module(module_name, state);
    compile_functions(module_path, state);
    return module_path;
}
