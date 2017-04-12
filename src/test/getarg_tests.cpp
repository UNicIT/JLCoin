#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-JLC");
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", false));
    BOOST_CHECK(GetBoolArg("-JLC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-JLCo"));
    BOOST_CHECK(!GetBoolArg("-JLCo", false));
    BOOST_CHECK(GetBoolArg("-JLCo", true));

    ResetArgs("-JLC=0");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", false));
    BOOST_CHECK(!GetBoolArg("-JLC", true));

    ResetArgs("-JLC=1");
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", false));
    BOOST_CHECK(GetBoolArg("-JLC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noJLC");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", false));
    BOOST_CHECK(!GetBoolArg("-JLC", true));

    ResetArgs("-noJLC=1");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", false));
    BOOST_CHECK(!GetBoolArg("-JLC", true));

    ResetArgs("-JLC -noJLC");  // -JLC should win
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", false));
    BOOST_CHECK(GetBoolArg("-JLC", true));

    ResetArgs("-JLC=1 -noJLC=1");  // -JLC should win
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", false));
    BOOST_CHECK(GetBoolArg("-JLC", true));

    ResetArgs("-JLC=0 -noJLC=0");  // -JLC should win
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", false));
    BOOST_CHECK(!GetBoolArg("-JLC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--JLC=1");
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", false));
    BOOST_CHECK(GetBoolArg("-JLC", true));

    ResetArgs("--noJLC=1");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", false));
    BOOST_CHECK(!GetBoolArg("-JLC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-JLC", "eleven"), "eleven");

    ResetArgs("-JLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-JLC", "eleven"), "");

    ResetArgs("-JLC=");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-JLC", "eleven"), "");

    ResetArgs("-JLC=11");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-JLC", "eleven"), "11");

    ResetArgs("-JLC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-JLC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-JLC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-JLC", 0), 0);

    ResetArgs("-JLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-JLC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-JLC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-JLC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-JLC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-JLC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--JLC");
    BOOST_CHECK_EQUAL(GetBoolArg("-JLC"), true);

    ResetArgs("--JLC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-JLC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noJLC");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", true));
    BOOST_CHECK(!GetBoolArg("-JLC", false));

    ResetArgs("-noJLC=1");
    BOOST_CHECK(!GetBoolArg("-JLC"));
    BOOST_CHECK(!GetBoolArg("-JLC", true));
    BOOST_CHECK(!GetBoolArg("-JLC", false));

    ResetArgs("-noJLC=0");
    BOOST_CHECK(GetBoolArg("-JLC"));
    BOOST_CHECK(GetBoolArg("-JLC", true));
    BOOST_CHECK(GetBoolArg("-JLC", false));

    ResetArgs("-JLC --noJLC");
    BOOST_CHECK(GetBoolArg("-JLC"));

    ResetArgs("-noJLC -JLC"); // JLC always wins:
    BOOST_CHECK(GetBoolArg("-JLC"));
}

BOOST_AUTO_TEST_SUITE_END()
