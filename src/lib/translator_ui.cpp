#include "translator_ui.hpp"
#include "cutvm_translator.hpp"
#include "lang_cutvm_translator.hpp"
#include "dictionary_methods.hpp"

using namespace cuttle;

void fileui::get_language_translator(compile_state &state, const language_t &from, const language_t &to, translator_t &translator) {
    if (to.name == "cutvm-cache" && to.version == 1) {
        lang::get_cutvm_translator(translator);
    } else if (from.name == "cutc-parser" && from.version == 1 && to.name == "cutvm" && to.version == 1) {
        lang::get_lang_cutvm_translator(translator);
    } else if (to.name == TRANSLATOR_ANY_NAME && to.version == TRANSLATOR_ANY_VERSION) {
        initialize(translator.dictionary);
        translator.to = to;
        translator.from = from;
    } else {
        initialize(translator.dictionary);
        // TODO: search module...
    }
}
