#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <string>
#include "test_utils.hpp"

BOOST_AUTO_TEST_SUITE(file_access_suite)

    BOOST_AUTO_TEST_CASE(can_read_and_write_files)
    {
        std::string file_path = (create_tmp() / "test_file_access.txt").string();
        std::string data = "This is a test data";
        std::ofstream os(file_path, std::ios::out);
        os << data;
        os.close();

        std::string str;
        std::ifstream is(file_path, std::ios::in);
        getline(is, str);
        BOOST_CHECK_EQUAL(str, data);
        is.close();
    }

BOOST_AUTO_TEST_SUITE_END()