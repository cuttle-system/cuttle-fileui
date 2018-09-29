#pragma once

#include <stdexcept>
#include <boost/filesystem.hpp>

namespace cuttle {
	namespace fileui {
		class format_error : public std::logic_error {
		public:
			format_error() : logic_error("Incorrect format error") {}

            explicit format_error(const std::string &message, const boost::filesystem::path &path)
					: logic_error("Incorrect format error: " + message + " at '" + path.string() + "'") {}
		};
	}
}