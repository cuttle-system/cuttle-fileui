#pragma once

#include <boost/filesystem.hpp>
#include "compile_state.hpp"
#include "generator_config.hpp"

namespace cuttle {
    namespace fileui {
        void get_generator_from_module(compile_state_t &state, const boost::filesystem::path &module_path,
                                       generator_config_t &generator_config);
    }
}