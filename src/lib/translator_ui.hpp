#pragma once

#include "compile_state.hpp"
#include "translator.hpp"

namespace cuttle {
    namespace fileui {
        void get_language_translator(compile_state &state, const language_t &from, const language_t &to, translator_t &translator);
    }
}