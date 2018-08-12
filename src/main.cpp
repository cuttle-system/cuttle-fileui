#include <cstring>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "module_compiler.hpp"
#include "compile_state.hpp"

#define CMD_MESSAGE argv[0] << " module_name [-mp module_path1:modulepath2] [(-h|--help)]" << std::endl

#define HELP_MESSAGE \
	"Usage:" << std::endl \
	<< "\tmodule_name - name of the module to compile" << std::endl \
	<< "\t-mp module_path - module search paths (use ':' to separate them, e.g /foo/bar:foobar/baz)" << std::endl \
	<< "\t-h|--help - display help message and exit"

#define BAD_ARGS_ERROR_MESSAGE \
	"Invalid args"

#define SUCCESS_MESSAGE \
    "You can see compiled version of your module at " \
    << cuttle::fileui::get_compiled_module_path(module_path) << std::endl;

void parse_args(int argc, char *argv[], char **module_name, char **module_path) {
	using namespace std;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0
			|| strcmp(argv[i], "-h") == 0
		) {
			std::cout << CMD_MESSAGE << std::endl;
			std::cout << HELP_MESSAGE;
			exit(0);
		} else if (strcmp(argv[i], "-mp") == 0) {
		    ++i;
		    if (i < argc) {
                if (*module_path) {
                    std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
                    std::cout << HELP_MESSAGE;
                    exit(0);
                }
                *module_path = argv[i];
            } else {
                std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
                std::cout << HELP_MESSAGE;
                exit(0);
		    }
		} else {
		    if (*module_name) {
                std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
                std::cout << HELP_MESSAGE;
                exit(0);
		    }
            *module_name = argv[i];
        }
    }
    if (!*module_path || !*module_name) {
        std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
        std::cout << HELP_MESSAGE;
        exit(0);
    }
}

void construct_search_path(char *c_module_path, std::list<boost::filesystem::path> &search_path) {
    search_path.push_back(boost::filesystem::current_path());

    if (c_module_path) {
        c_module_path = strdup(c_module_path);
        char *path = strtok(c_module_path, ":");
        while (path) {
            search_path.emplace_back(path);
            path = strtok(nullptr, ":");
        }
        free(c_module_path);
    }
}

int main(int argc, char *argv[]) {
    char *c_module_name = nullptr, *c_module_path = nullptr;
	parse_args(argc, argv, &c_module_name, &c_module_path);

    std::list<boost::filesystem::path> search_path;
    construct_search_path(c_module_path, search_path);

    cuttle::fileui::compile_state state {search_path};
    auto module_path = cuttle::fileui::compile_module(c_module_name, state);
    std::cout << SUCCESS_MESSAGE;
}