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
    ResetArgs("-LOC");
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", false));
    BOOST_CHECK(GetBoolArg("-LOC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-LOCo"));
    BOOST_CHECK(!GetBoolArg("-LOCo", false));
    BOOST_CHECK(GetBoolArg("-LOCo", true));

    ResetArgs("-LOC=0");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", false));
    BOOST_CHECK(!GetBoolArg("-LOC", true));

    ResetArgs("-LOC=1");
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", false));
    BOOST_CHECK(GetBoolArg("-LOC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noLOC");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", false));
    BOOST_CHECK(!GetBoolArg("-LOC", true));

    ResetArgs("-noLOC=1");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", false));
    BOOST_CHECK(!GetBoolArg("-LOC", true));

    ResetArgs("-LOC -noLOC");  // -LOC should win
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", false));
    BOOST_CHECK(GetBoolArg("-LOC", true));

    ResetArgs("-LOC=1 -noLOC=1");  // -LOC should win
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", false));
    BOOST_CHECK(GetBoolArg("-LOC", true));

    ResetArgs("-LOC=0 -noLOC=0");  // -LOC should win
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", false));
    BOOST_CHECK(!GetBoolArg("-LOC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--LOC=1");
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", false));
    BOOST_CHECK(GetBoolArg("-LOC", true));

    ResetArgs("--noLOC=1");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", false));
    BOOST_CHECK(!GetBoolArg("-LOC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LOC", "eleven"), "eleven");

    ResetArgs("-LOC -bar");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LOC", "eleven"), "");

    ResetArgs("-LOC=");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LOC", "eleven"), "");

    ResetArgs("-LOC=11");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-LOC", "eleven"), "11");

    ResetArgs("-LOC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-LOC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LOC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-LOC", 0), 0);

    ResetArgs("-LOC -bar");
    BOOST_CHECK_EQUAL(GetArg("-LOC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-LOC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-LOC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-LOC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-LOC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--LOC");
    BOOST_CHECK_EQUAL(GetBoolArg("-LOC"), true);

    ResetArgs("--LOC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-LOC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noLOC");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", true));
    BOOST_CHECK(!GetBoolArg("-LOC", false));

    ResetArgs("-noLOC=1");
    BOOST_CHECK(!GetBoolArg("-LOC"));
    BOOST_CHECK(!GetBoolArg("-LOC", true));
    BOOST_CHECK(!GetBoolArg("-LOC", false));

    ResetArgs("-noLOC=0");
    BOOST_CHECK(GetBoolArg("-LOC"));
    BOOST_CHECK(GetBoolArg("-LOC", true));
    BOOST_CHECK(GetBoolArg("-LOC", false));

    ResetArgs("-LOC --noLOC");
    BOOST_CHECK(GetBoolArg("-LOC"));

    ResetArgs("-noLOC -LOC"); // LOC always wins:
    BOOST_CHECK(GetBoolArg("-LOC"));
}

BOOST_AUTO_TEST_SUITE_END()
