#include "file_interpreter.hpp"
#include "interpreter.hpp"

using namespace cuttle;

namespace fs = boost::filesystem;

void fileui::interpret_file(vm::context_t &context, const fs::path &file_path, std::deque<vm::value_t> &arg_stack) {
    std::ifstream file(file_path.string());
    cuttle::vm::interpret(file, context, arg_stack);
    file.close();
}