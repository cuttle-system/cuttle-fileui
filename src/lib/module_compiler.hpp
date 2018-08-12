#pragma once

#include <string>
#include <list>
#include <boost/filesystem.hpp>
#include "compile_state.hpp"

#define COMPILED_PATH_PREFIX "cutvm.compiled"

namespace cuttle {
	namespace fileui {
        boost::filesystem::path get_compiled_module_path(const boost::filesystem::path &module_path);
        boost::filesystem::path get_compiled_function_path(const boost::filesystem::path &function_path);

		boost::filesystem::path search_module(
            const std::string &module_name,
            const cuttle::fileui::compile_state& state);

		boost::filesystem::path compile_module(
            const std::string &module_name,
            cuttle::fileui::compile_state& state);

        void compile_functions(
            const boost::filesystem::path &module_path,
            cuttle::fileui::compile_state& state);

        void compile_function(
            const boost::filesystem::path &function_path,
			const boost::filesystem::path &module_path,
            cuttle::fileui::compile_state& state);
	}
}
