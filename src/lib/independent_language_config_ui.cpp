#include "independent_language_config_ui.hpp"
#include "lang_tokenizer_cutc_parser.hpp"
#include "lang_tokenizer.hpp"
#include "lang_parser_cutc_parser.hpp"
#include "lang_generator_cutc_parser.hpp"
#include "lang_generator_arg_cutc_parser.hpp"
#include "lang_macro_functions_parser.hpp"
#include "cutvm_context.hpp"
#include "cutvm_tokenizer.hpp"
#include "cutvm_generator.hpp"
#include "fileui_module.hpp"
#include "tokenizer_ui.hpp"
#include "parser_ui.hpp"
#include "generator_ui.hpp"

using namespace cuttle;

void fileui::get_independent_language_config(compile_state_t &state, const language_t &lang,
                                             context_t &context, tokenizer_config_t &tokenizer,
                                             generator_config_t &generator_config
) {
    if (lang.name == "cutc-tokenizer" && lang.version == 1) {
        lang::get_tokenizer_cutc_parser(context);
        lang::get_tokenizer_config(tokenizer);
    } else if (lang.name == "cutc-parser" && lang.version == 2) {
        lang::get_parser_cutc_parser(context);
        lang::get_tokenizer_config(tokenizer);
    } else if (lang.name == "cutc-generator" && lang.version == 1) {
        lang::get_generator_cutc_parser(context);
        lang::get_tokenizer_config(tokenizer);
    } else if (lang.name == "cutc-generator-arg" && lang.version == 1) {
        lang::get_generator_arg_cutc_parser(context);
        lang::get_tokenizer_config(tokenizer);
    } else if ((lang.name == "cutvm" || lang.name == "cutvm-cache" || lang.name == "cutvm-translator-output") && lang.version == 1) {
        lang::get_cutvm_context(context);
        lang::get_cutvm_tokenizer_config(tokenizer);
        lang::get_cutvm_generator_config(generator_config);
    } else {
        lang::get_macro_functions_parser(context);
        auto module_path = search_module(state, lang.name + "." + std::to_string(lang.version));
        get_tokenizer_from_module(state, module_path, tokenizer);
        get_context_from_module(state, module_path, context);
        get_generator_from_module(state, module_path, generator_config);
    }
}