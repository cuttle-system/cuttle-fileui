#pragma once

#include "compile_state.hpp"
#include "language.hpp"
#include "context.hpp"
#include "tokenizer_config.hpp"
#include "generator_config.hpp"

namespace cuttle {
    namespace fileui {
        void get_independent_language_config(compile_state_t &state, const language_t &lang,
                                             context_t &context, tokenizer_config_t &tokenizer,
                                             generator_config_t &generator_config
        );
    }
}