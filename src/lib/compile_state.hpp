#pragma once

#include <list>
#include <boost/filesystem.hpp>
#include "language.hpp"

namespace cuttle {
    namespace fileui {
        struct compile_state {
            std::list<boost::filesystem::path> search_path;
            std::vector<language_t> translator_target_languages;
        };
    }
}