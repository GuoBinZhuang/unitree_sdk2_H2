#ifndef __UT_TEST_HPP__
#define __UT_TEST_HPP__

/*
 * Minimal dependency free unit test framework.
 *
 * Usage:
 *     UT_TEST(TrimRemovesSpaces)
 *     {
 *         std::string s = "  a  ";
 *         UT_EXPECT_EQ(unitree::common::Trim(s), "a");
 *     }
 *
 *     UT_TEST_MAIN()
 */

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace unitree
{
namespace test
{
class TestCase
{
public:
    TestCase(const std::string& name, std::function<void()> func)
        : mName(name), mFunc(func)
    {}

    const std::string& GetName() const
    {
        return mName;
    }

    void Run() const
    {
        mFunc();
    }

private:
    std::string mName;
    std::function<void()> mFunc;
};

class TestRegistry
{
public:
    static TestRegistry& Instance()
    {
        static TestRegistry registry;
        return registry;
    }

    void Add(const TestCase& testCase)
    {
        mTestCases.push_back(testCase);
    }

    const std::vector<TestCase>& GetTestCases() const
    {
        return mTestCases;
    }

private:
    std::vector<TestCase> mTestCases;
};

class AssertionError
{
public:
    explicit AssertionError(const std::string& message)
        : mMessage(message)
    {}

    const std::string& GetMessage() const
    {
        return mMessage;
    }

private:
    std::string mMessage;
};

class TestRegistrar
{
public:
    TestRegistrar(const std::string& name, std::function<void()> func)
    {
        TestRegistry::Instance().Add(TestCase(name, func));
    }
};

template<typename T>
std::string Describe(const T& value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

static inline std::string Describe(bool value)
{
    return value ? "true" : "false";
}

static inline int32_t RunAllTests()
{
    const std::vector<TestCase>& testCases = TestRegistry::Instance().GetTestCases();

    int32_t failed = 0;

    for (const TestCase& testCase : testCases)
    {
        std::string error;

        try
        {
            testCase.Run();
        }
        catch (const AssertionError& e)
        {
            error = e.GetMessage();
        }
        catch (const std::exception& e)
        {
            error = std::string("unexpected std::exception: ") + e.what();
        }
        catch (...)
        {
            error = "unexpected unknown exception";
        }

        if (error.empty())
        {
            std::cout << "[ PASS ] " << testCase.GetName() << std::endl;
        }
        else
        {
            failed++;
            std::cout << "[ FAIL ] " << testCase.GetName() << std::endl
                << "         " << error << std::endl;
        }
    }

    std::cout << "[ ---- ] " << (testCases.size() - failed) << "/"
        << testCases.size() << " test(s) passed" << std::endl;

    return failed == 0 ? 0 : 1;
}
}
}

#define UT_TEST_LOCATION() \
    (std::string(__FILE__) + ":" + std::to_string(__LINE__))

#define UT_TEST_FAIL(message) \
    throw unitree::test::AssertionError(UT_TEST_LOCATION() + ": " + (message))

#define UT_EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { UT_TEST_FAIL(std::string("expected true: ") + #expr); } \
    } while (0)

#define UT_EXPECT_FALSE(expr) \
    do { \
        if ((expr)) { UT_TEST_FAIL(std::string("expected false: ") + #expr); } \
    } while (0)

#define UT_EXPECT_EQ(actual, expected) \
    do { \
        auto&& __ut_actual = (actual); \
        auto&& __ut_expected = (expected); \
        if (!(__ut_actual == __ut_expected)) { \
            UT_TEST_FAIL(std::string(#actual) + " is " \
                + unitree::test::Describe(__ut_actual) + ", expected " \
                + unitree::test::Describe(__ut_expected)); \
        } \
    } while (0)

#define UT_EXPECT_NE(actual, unexpected) \
    do { \
        auto&& __ut_actual = (actual); \
        auto&& __ut_unexpected = (unexpected); \
        if ((__ut_actual == __ut_unexpected)) { \
            UT_TEST_FAIL(std::string(#actual) + " is " \
                + unitree::test::Describe(__ut_actual) + ", expected another value"); \
        } \
    } while (0)

#define UT_EXPECT_NEAR(actual, expected, tolerance) \
    do { \
        double __ut_actual = (double)(actual); \
        double __ut_expected = (double)(expected); \
        if (std::fabs(__ut_actual - __ut_expected) > (double)(tolerance)) { \
            UT_TEST_FAIL(std::string(#actual) + " is " \
                + unitree::test::Describe(__ut_actual) + ", expected " \
                + unitree::test::Describe(__ut_expected) + " within " \
                + unitree::test::Describe((double)(tolerance))); \
        } \
    } while (0)

#define UT_EXPECT_THROW(expr, exceptionType) \
    do { \
        bool __ut_thrown = false; \
        try { expr; } \
        catch (const exceptionType&) { __ut_thrown = true; } \
        if (!__ut_thrown) { \
            UT_TEST_FAIL(std::string("expected ") + #exceptionType \
                + " thrown by: " + #expr); \
        } \
    } while (0)

#define UT_TEST(name) \
    static void UtTestBody_##name(); \
    static unitree::test::TestRegistrar UtTestRegistrar_##name(#name, UtTestBody_##name); \
    static void UtTestBody_##name()

#define UT_TEST_MAIN() \
    int main() \
    { \
        return unitree::test::RunAllTests(); \
    }

#endif//__UT_TEST_HPP__
