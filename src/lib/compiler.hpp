#pragma once

#include <string>
#include <list>
#include <boost/filesystem.hpp>
#include "compile_state.hpp"

#define CUTTLE_FILEUI_COMPILED_PATH_POSTFIX ".cutvm.compiled"
#define CUTTLE_FILEUI_ROOT_PATH_FILE ".cutroot"

namespace cuttle {
	namespace fileui {
        boost::filesystem::path get_parent_module_path(const boost::filesystem::path &path);
        boost::filesystem::path get_compiled_module_path(const boost::filesystem::path &module_path);
        boost::filesystem::path get_compiled_file_path(const boost::filesystem::path &function_path);

		boost::filesystem::path search_module(
		        const compile_state &state,
		        const std::string &module_name);

        void compile_file(compile_state &state, const boost::filesystem::path &file_path,
                          const boost::filesystem::path &compiled_file_path = "");

		void compile_files(compile_state &state, const boost::filesystem::path &module_path);
	}
}
