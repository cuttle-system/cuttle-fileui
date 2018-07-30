#include <cstring>
#include <iostream>

#define CMD_MESSAGE argv[0] << " module_path [(-h|--help)]" << std::endl

#define HELP_MESSAGE \
	"Usage:" << std::endl \
	<< "\tmodule_path - path to the module to compile" << std::endl \
	<< "\t-h|--help - display help message and exit"

#define BAD_ARGS_ERROR_MESSAGE \
	"Invalid args"

void parse_args(int argc, char *argv[], char **modulePath) {
	using namespace std;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0
			|| strcmp(argv[i], "-h") == 0
		) {
			std::cout << CMD_MESSAGE << std::endl;
			std::cout << HELP_MESSAGE;
			exit(0);
		}
	}
	if (argc != 2) {
		std::cout << CMD_MESSAGE;
		std::cout << BAD_ARGS_ERROR_MESSAGE;
		exit(0);
	}
	*modulePath = argv[1];
}

int main(int argc, char *argv[]) {
	char *modulePath = nullptr;
	parse_args(argc, argv, &modulePath);

}