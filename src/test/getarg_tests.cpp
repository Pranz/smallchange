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
    vecArg.insert(vecArg.begin(), "testtoakrona");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-TOA");
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", false));
    BOOST_CHECK(GetBoolArg("-TOA", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-TOAo"));
    BOOST_CHECK(!GetBoolArg("-TOAo", false));
    BOOST_CHECK(GetBoolArg("-TOAo", true));

    ResetArgs("-TOA=0");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", false));
    BOOST_CHECK(!GetBoolArg("-TOA", true));

    ResetArgs("-TOA=1");
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", false));
    BOOST_CHECK(GetBoolArg("-TOA", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noTOA");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", false));
    BOOST_CHECK(!GetBoolArg("-TOA", true));

    ResetArgs("-noTOA=1");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", false));
    BOOST_CHECK(!GetBoolArg("-TOA", true));

    ResetArgs("-TOA -noTOA");  // -TOA should win
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", false));
    BOOST_CHECK(GetBoolArg("-TOA", true));

    ResetArgs("-TOA=1 -noTOA=1");  // -TOA should win
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", false));
    BOOST_CHECK(GetBoolArg("-TOA", true));

    ResetArgs("-TOA=0 -noTOA=0");  // -TOA should win
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", false));
    BOOST_CHECK(!GetBoolArg("-TOA", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--TOA=1");
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", false));
    BOOST_CHECK(GetBoolArg("-TOA", true));

    ResetArgs("--noTOA=1");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", false));
    BOOST_CHECK(!GetBoolArg("-TOA", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TOA", "eleven"), "eleven");

    ResetArgs("-TOA -bar");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TOA", "eleven"), "");

    ResetArgs("-TOA=");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TOA", "eleven"), "");

    ResetArgs("-TOA=11");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-TOA", "eleven"), "11");

    ResetArgs("-TOA=eleven");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-TOA", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TOA", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-TOA", 0), 0);

    ResetArgs("-TOA -bar");
    BOOST_CHECK_EQUAL(GetArg("-TOA", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-TOA=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-TOA", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-TOA=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-TOA", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--TOA");
    BOOST_CHECK_EQUAL(GetBoolArg("-TOA"), true);

    ResetArgs("--TOA=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-TOA", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noTOA");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", true));
    BOOST_CHECK(!GetBoolArg("-TOA", false));

    ResetArgs("-noTOA=1");
    BOOST_CHECK(!GetBoolArg("-TOA"));
    BOOST_CHECK(!GetBoolArg("-TOA", true));
    BOOST_CHECK(!GetBoolArg("-TOA", false));

    ResetArgs("-noTOA=0");
    BOOST_CHECK(GetBoolArg("-TOA"));
    BOOST_CHECK(GetBoolArg("-TOA", true));
    BOOST_CHECK(GetBoolArg("-TOA", false));

    ResetArgs("-TOA --noTOA");
    BOOST_CHECK(GetBoolArg("-TOA"));

    ResetArgs("-noTOA -TOA"); // TOA always wins:
    BOOST_CHECK(GetBoolArg("-TOA"));
}

BOOST_AUTO_TEST_SUITE_END()
