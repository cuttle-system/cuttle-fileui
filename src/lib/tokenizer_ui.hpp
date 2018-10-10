#pragma once

#include "compile_state.hpp"
#include <boost/filesystem.hpp>
#include "tokenizer_config.hpp"

namespace cuttle {
    namespace fileui {
        void get_tokenizer_from_module(compile_state &state, const boost::filesystem::path &module_path,
                                       tokenizer_config_t &tokenizer);
    }
}