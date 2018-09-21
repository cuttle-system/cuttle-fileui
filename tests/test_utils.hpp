#pragma once

#include <ctime>
#include <chrono>
#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

inline std::string get_current_datetime() {
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    std::string timestr = asctime(timeinfo);
    if (timestr.back() == '\n') {
        timestr.pop_back();
    }
    for (auto &ch : timestr) {
        if (ch == ':' || ch == ' ') ch = '_';
    }
    return timestr;
}

inline boost::filesystem::path create_tmp() {
    using namespace boost::unit_test::framework;
    std::string datetime = get_current_datetime();
    std::string test_case_name = current_test_case().p_name;
    auto tmp = boost::filesystem::temp_directory_path()
            / "cuttle-fileui"
            / datetime
            / test_case_name
            / boost::filesystem::unique_path();
    create_directories(tmp);
    return tmp;
}
