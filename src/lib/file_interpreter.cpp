#include <sstream>
#include "file_interpreter.hpp"
#include "interpreter.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void fileui::interpret_file(compile_state_t &state, vm::context_t &context, const fs::path &file_path, std::deque<vm::value_t> &arg_stack) {
    if (state.cached_files.count(file_path.string())) {
        std::stringstream input_str (state.cached_files[file_path.string()]);
        cuttle::vm::interpret(input_str, context, arg_stack);
    } else {
        std::ifstream file(file_path.string());
        cuttle::vm::interpret(file, context, arg_stack);
        file.close();
//        std::ifstream file1(file_path.string());
//        state.cached_files[file_path.string()] = std::string((std::istreambuf_iterator<char>(file1)),
//                                                             std::istreambuf_iterator<char>());
//        file1.close();
    }
}