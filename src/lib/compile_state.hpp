#pragma once

#include <list>
#include <boost/filesystem.hpp>
#include "language.hpp"
#include "vm_context.hpp"

namespace cuttle {
    namespace fileui {
        struct compile_state_t {
            std::list<boost::filesystem::path> search_path;
            std::vector<language_t> translator_target_languages;
        };
    }
}