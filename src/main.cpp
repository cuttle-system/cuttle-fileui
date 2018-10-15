#include <cstring>
#include <string>
#include <iostream>
#include <boost/filesystem.hpp>
#include "compiler.hpp"
#include "fileui_file.hpp"
#include "compile_state.hpp"

#define CMD_MESSAGE argv[0] << " module_name [-mp module_path1:modulepath2] [(-h|--help)]" << std::endl

#define HELP_MESSAGE \
	"Usage:" << std::endl \
	<< "    input_file_path     - path to file to compile" << std::endl \
	<< "    -mp module_path - module search paths (use ':' to separate them, e.g /foo/bar:foobar/baz)" << std::endl \
	<< "    -h|--help       - display help message and exit" << std::endl

#define BAD_ARGS_ERROR_MESSAGE \
	"Invalid args"

#define SUCCESS_MESSAGE \
    "You can see compiled version of your file at " \
    << cuttle::fileui::get_output_file_path(*c_file_path) << std::endl;

void parse_args(int argc, char *argv[], boost::filesystem::path **file_path, char **module_path) {
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
		    if (*file_path) {
                std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
                std::cout << HELP_MESSAGE;
                exit(0);
		    }
            *file_path = new boost::filesystem::path(boost::filesystem::absolute(boost::filesystem::path(argv[i])));

        }
    }
    if (!*module_path || !*file_path) {
        std::cout << BAD_ARGS_ERROR_MESSAGE << std::endl;
        std::cout << HELP_MESSAGE;
        exit(0);
    }
}

void construct_search_path(char *c_module_path, std::list<boost::filesystem::path> &search_path) {
    search_path.push_back(boost::filesystem::current_path());

    if (c_module_path) {
        c_module_path = strdup(c_module_path);
        char *saveptr1;
        char *path = strtok_r(c_module_path, ":", &saveptr1);
        while (path) {
            search_path.emplace_back(boost::filesystem::absolute(path));
            path = strtok_r(nullptr, ":", &saveptr1);
        }
        free(c_module_path);
    }
}

int main(int argc, char *argv[]) {
//    setenv("LC_ALL", "C", 1);

    boost::filesystem::path *c_file_path = nullptr;
    char *c_module_path = nullptr;
	parse_args(argc, argv, &c_file_path, &c_module_path);

    std::list<boost::filesystem::path> search_path;
    construct_search_path(c_module_path, search_path);

    cuttle::fileui::compile_state_t state {search_path};
    cuttle::fileui::compile_file(state, *c_file_path, c_file_path->string() + ".cached", c_file_path->string() + ".output");
    std::cout << SUCCESS_MESSAGE;
}
