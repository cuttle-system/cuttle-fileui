#pragma once

#include <string>

#define TESTCASE std::cout << __func__ << std::endl;

#define AssertTrue(statement, message)                  \
    cuttle::_AssertTrue((statement), message, __FILE__, __LINE__)

#define AssertEqual(out, exp, message)					\
	cuttle::_AssertTrue((out) == (exp), message,  __FILE__, __LINE__)

#define AssertNotEqual(out, exp, message)					\
	cuttle::_AssertTrue((out) != (exp), message,  __FILE__, __LINE__)

#define AssertThrows(exception, ...) { \
		try { \
			__VA_ARGS__ \
			AssertTrue(false, "Exception was not thrown"); \
		} catch(exception& exc) { \
			(void) exc; \
		} \
	}

namespace cuttle {
    void _AssertTrue(
        bool statement, const std::string& message, const char* file, int line
    );

	template <typename T>
	void _AssertEqual(
		T out, T exp,
		const std::string& message, const char* file, int line
	) {
		
	}

    void Log(const std::string &message);
}
