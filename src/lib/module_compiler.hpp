#pragma once

#include <string>
#include <list>
#include <boost/filesystem.hpp>

namespace cuttle {
	namespace fileui {
		boost::filesystem::path compile_module(
            const std::string &module_name,
            const std::list<boost::filesystem::path> &search_path);

        boost::filesystem::path search_module(
            const std::string &module_name,
            const std::list<boost::filesystem::path> &search_path);

        boost::filesystem::path compile_function(
            const boost::filesystem::path &function_path,
            const boost::filesystem::path &module_path,
            const boost::filesystem::path &compiled_module_path);

        void compile_functions(
            const boost::filesystem::path &module_path,
            const boost::filesystem::path &compiled_module_path);
	}
}
