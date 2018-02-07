#include <iostream>
#include <stdexcept>
#include "test_manager.hpp"

using namespace std;

int main(void)
{
	try {
		run_tests();
		cout << "ok" << endl;
	}
	catch (std::logic_error& e) {
		cout << "logic_error: " << e.what() << endl;
	}
#ifdef _WIN32
	system("pause");
#endif
}
