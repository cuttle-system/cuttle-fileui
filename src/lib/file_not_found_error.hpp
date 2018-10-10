#pragma once

#include <stdexcept>
#include <boost/filesystem.hpp>

namespace cuttle {
	namespace fileui {
		class file_not_found_error : public std::runtime_error {
		public:
            explicit file_not_found_error(const boost::filesystem::path &path)
					: runtime_error("File '" + path.string() + "' was not found") {}
		};
	}
}