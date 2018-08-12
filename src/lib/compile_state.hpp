#pragma once

#include <list>
#include <boost/filesystem.hpp>

namespace cuttle {
    namespace fileui {
        struct compile_state {
            std::list<boost::filesystem::path> search_path;
        };
    }
}