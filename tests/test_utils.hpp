#pragma once

#include <ctime>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

using namespace boost::filesystem;

inline std::string get_current_datetime() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    return asctime(timeinfo);
}

inline path create_tmp() {
    using namespace boost::unit_test::framework;
    std::string datetime = get_current_datetime();
    std::string test_case_name = current_test_case().p_name;
    auto tmp = temp_directory_path() / datetime / test_case_name / unique_path();
    create_directories(tmp);
    return tmp;
}
