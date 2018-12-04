#pragma once

#include <list>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include "language.hpp"
#include "vm_context.hpp"
#include "tokenizer_config.hpp"
#include "generator_config.hpp"
#include "call_tree.hpp"
#include "token.hpp"

namespace cuttle {
    namespace fileui {
        struct compile_state_t {
            std::unordered_map<std::string, std::string> cached_files;
            std::list<boost::filesystem::path> search_path;
            std::unordered_map<std::string, call_tree_t> cached_trees;
            std::unordered_map<std::string, tokens_t> cached_tokens;
            std::unordered_map<std::string, tokenizer_config_t> cached_tokenizer_configs;
            std::unordered_map<std::string, generator_config_t> cached_generator_configs;
            std::vector<language_t> translator_target_languages;
        };
    }
}