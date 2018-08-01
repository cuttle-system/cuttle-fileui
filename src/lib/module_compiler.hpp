#pragma once

#include <string>
#include <list>

namespace cuttle {
	namespace fileui {
		void compile_module(const std::string &module_name, const std::list<boost::filesystem::path> &search_path);
        boost::filesystem::path search_module(const std::string& module_name, const std::list<boost::filesystem::path>& search_path);
	}
}