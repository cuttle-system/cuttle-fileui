#pragma once

#include <stdexcept>
#include <boost/filesystem.hpp>

namespace cuttle {
	namespace fileui {
		class incorrect_module_structure_error : public std::runtime_error {
		public:
			incorrect_module_structure_error() : runtime_error("Incorrect module structure error") {}

            explicit incorrect_module_structure_error(const boost::filesystem::path &path)
					: runtime_error("Incorrect module structure error at '" + path.string() + "'") {}
		};
	}
}