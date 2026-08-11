#include <unitree/common/string_tool.hpp>

#include "ut_test.hpp"

using namespace unitree::common;

UT_TEST(ToStringHandlesCharPointerStringAndNumber)
{
    UT_EXPECT_EQ(ToString("abc"), std::string("abc"));
    UT_EXPECT_EQ(ToString(std::string("abc")), std::string("abc"));
    UT_EXPECT_EQ(ToString<int32_t>(-12), std::string("-12"));
    UT_EXPECT_EQ(ToString<uint64_t>(12), std::string("12"));
}

UT_TEST(StringToParsesIntegerTypes)
{
    UT_EXPECT_EQ(StringTo<int32_t>("-2147483648"), INT32_MIN);
    UT_EXPECT_EQ(StringTo<uint32_t>("4294967295"), UINT32_MAX);
    UT_EXPECT_EQ(StringTo<int64_t>("-9223372036854775807"), (int64_t)-9223372036854775807LL);
    UT_EXPECT_EQ(StringTo<uint64_t>("18446744073709551615"), UINT64_MAX);
}

UT_TEST(StringToParsesFloatingTypes)
{
    UT_EXPECT_NEAR(StringTo<float>("1.25"), 1.25, 1e-6);
    UT_EXPECT_NEAR(StringTo<double>("-3.5e2"), -350.0, 1e-9);
    UT_EXPECT_NEAR(StringTo<long double>("0.5"), 0.5, 1e-9);
}

UT_TEST(ToUpperAndToLowerModifyInPlace)
{
    std::string s = "aBc1-";

    UT_EXPECT_EQ(ToUpper(s), std::string("ABC1-"));
    UT_EXPECT_EQ(s, std::string("ABC1-"));

    UT_EXPECT_EQ(ToLower(s), std::string("abc1-"));
    UT_EXPECT_EQ(s, std::string("abc1-"));
}

UT_TEST(CompareRespectsCaseSensitivity)
{
    UT_EXPECT_EQ(Compare("abc", "abc"), 0);
    UT_EXPECT_TRUE(Compare("abc", "ABC") != 0);
    UT_EXPECT_EQ(Compare("abc", "ABC", false), 0);
    UT_EXPECT_TRUE(Compare("abc", "abd") < 0);
    UT_EXPECT_TRUE(Compare("abd", "abc") > 0);
}

UT_TEST(CompareSubstringUsesPositionAndLength)
{
    UT_EXPECT_EQ(Compare("xxabc", 2, 3, "abc"), 0);
    UT_EXPECT_EQ(Compare("xxABC", 2, 3, "abc", false), 0);
    UT_EXPECT_TRUE(Compare("xxabc", 2, 3, "abd") != 0);
}

UT_TEST(TrimLeftRemovesLeadingDelimitersOnly)
{
    std::string s = " \t\nabc \t";
    UT_EXPECT_EQ(TrimLeft(s), std::string("abc \t"));

    std::string custom = "xxabcxx";
    UT_EXPECT_EQ(TrimLeft(custom, "x"), std::string("abcxx"));
}

UT_TEST(TrimRightRemovesTrailingDelimitersOnly)
{
    std::string s = " \tabc \n";
    UT_EXPECT_EQ(TrimRight(s), std::string(" \tabc"));

    std::string custom = "xxabcxx";
    UT_EXPECT_EQ(TrimRight(custom, "x"), std::string("xxabc"));
}

UT_TEST(TrimRemovesBothSides)
{
    std::string s = "\r\n abc \t";
    UT_EXPECT_EQ(Trim(s), std::string("abc"));

    std::string blank = " \t\r\n";
    UT_EXPECT_EQ(Trim(blank), std::string(""));

    std::string empty = "";
    UT_EXPECT_EQ(Trim(empty), std::string(""));
}

UT_TEST(SplitBreaksOnDelimiter)
{
    std::vector<std::string> parts;
    Split("a,b,c", parts, ",");

    UT_EXPECT_EQ(parts.size(), (size_t)3);
    UT_EXPECT_EQ(parts[0], std::string("a"));
    UT_EXPECT_EQ(parts[1], std::string("b"));
    UT_EXPECT_EQ(parts[2], std::string("c"));
}

UT_TEST(SplitWithoutDelimiterKeepsWholeString)
{
    std::vector<std::string> parts;
    Split("abc", parts, ",");

    UT_EXPECT_EQ(parts.size(), (size_t)1);
    UT_EXPECT_EQ(parts[0], std::string("abc"));
}

UT_TEST(StartWithChecksPrefix)
{
    UT_EXPECT_TRUE(StartWith("abcdef", "abc"));
    UT_EXPECT_FALSE(StartWith("abcdef", "bcd"));
    UT_EXPECT_FALSE(StartWith("abcdef", "ABC"));
    UT_EXPECT_TRUE(StartWith("abcdef", "ABC", false));
    UT_EXPECT_FALSE(StartWith("ab", "abc"));
}

UT_TEST(EndWithChecksSuffix)
{
    UT_EXPECT_TRUE(EndWith("abcdef", "def"));
    UT_EXPECT_FALSE(EndWith("abcdef", "cde"));
    UT_EXPECT_FALSE(EndWith("abcdef", "DEF"));
    UT_EXPECT_TRUE(EndWith("abcdef", "DEF", false));
    UT_EXPECT_FALSE(EndWith("ef", "def"));
}

UT_TEST(ReplaceSubstitutesAllOccurrences)
{
    UT_EXPECT_EQ(Replace("a.b.c", ".", "-"), std::string("a-b-c"));
    UT_EXPECT_EQ(Replace("abc", "x", "y"), std::string("abc"));
    UT_EXPECT_EQ(Replace("aaa", "aa", "b"), std::string("ba"));
}

UT_TEST_MAIN()
