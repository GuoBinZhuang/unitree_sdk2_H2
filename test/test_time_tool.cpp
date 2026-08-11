#include <unitree/common/time/sleep.hpp>
#include <unitree/common/time/time_tool.hpp>

#include "ut_test.hpp"

using namespace unitree::common;

UT_TEST(GetCurrentTimeUnitsAreConsistent)
{
    uint64_t sec = GetCurrentTime();
    uint64_t millisec = GetCurrentTimeMillisecond();
    uint64_t microsec = GetCurrentTimeMicrosecond();
    uint64_t nanosec = GetCurrentTimeNanosecond();

    UT_EXPECT_TRUE(sec > 1600000000);
    UT_EXPECT_TRUE(millisec / UT_NUMER_MILLI >= sec);
    UT_EXPECT_TRUE(microsec / UT_NUMER_MICRO >= sec);
    UT_EXPECT_TRUE(nanosec / UT_NUMER_NANO >= sec);
}

UT_TEST(MonotonicTimeIsNonDecreasing)
{
    uint64_t first = GetCurrentMonotonicTimeMicrosecond();
    MicroSleep(1000);
    uint64_t second = GetCurrentMonotonicTimeMicrosecond();

    UT_EXPECT_TRUE(second >= first);
    UT_EXPECT_TRUE(GetCurrentMonotonicTimeNanosecond() >= second * UT_NUMER_MILLI);
}

/*
 * GetCurrentCpuTime[Nano|Micro]second are declared in time_tool.hpp but not
 * provided by the prebuilt library, so only the thread variants are covered.
 */
UT_TEST(ThreadCpuTimeIsPositive)
{
    UT_EXPECT_TRUE(GetCurrentThreadCpuTimeNanosecond() > 0);
    UT_EXPECT_TRUE(GetCurrentThreadCpuTimeMicrosecond() > 0);
}

UT_TEST(TimevalConversionRoundTrips)
{
    struct timeval tv;
    MicrosecondToTimeval(1234567, tv);

    UT_EXPECT_EQ((int64_t)tv.tv_sec, (int64_t)1);
    UT_EXPECT_EQ((int64_t)tv.tv_usec, (int64_t)234567);
    UT_EXPECT_EQ(TimevalToMicrosecond(tv), (uint64_t)1234567);
    UT_EXPECT_EQ(TimevalToMillisecond(tv), (uint64_t)1234);

    MillisecondToTimeval(1500, tv);
    UT_EXPECT_EQ((int64_t)tv.tv_sec, (int64_t)1);
    UT_EXPECT_EQ((int64_t)tv.tv_usec, (int64_t)500000);
    UT_EXPECT_EQ(TimevalToMillisecond(tv), (uint64_t)1500);
}

UT_TEST(TimespecConversionRoundTrips)
{
    struct timespec ts;
    MicrosecondToTimespec(1234567, ts);

    UT_EXPECT_EQ((int64_t)ts.tv_sec, (int64_t)1);
    UT_EXPECT_EQ((int64_t)ts.tv_nsec, (int64_t)234567000);
    UT_EXPECT_EQ(TimespecToMicrosecond(ts), (uint64_t)1234567);
    UT_EXPECT_EQ(TimespecToMillisecond(ts), (uint64_t)1234);

    MillisecondToTimespec(1500, ts);
    UT_EXPECT_EQ((int64_t)ts.tv_sec, (int64_t)1);
    UT_EXPECT_EQ((int64_t)ts.tv_nsec, (int64_t)500000000);
    UT_EXPECT_EQ(TimespecToMillisecond(ts), (uint64_t)1500);
}

UT_TEST(GetCurrentTimevalAndTimespecAreFilled)
{
    struct timeval tv;
    struct timespec ts;

    GetCurrentTimeval(tv);
    GetCurrentTimespec(ts);

    UT_EXPECT_TRUE(tv.tv_sec > 1600000000);
    UT_EXPECT_TRUE(ts.tv_sec > 1600000000);
    UT_EXPECT_TRUE(tv.tv_usec >= 0 && tv.tv_usec < UT_NUMER_MICRO);
    UT_EXPECT_TRUE(ts.tv_nsec >= 0 && ts.tv_nsec < UT_NUMER_NANO);
}

UT_TEST(TimeFormatStringFormatsEpoch)
{
    UT_EXPECT_EQ(TimeFormatString((uint64_t)0), std::string("1970-01-01 00:00:00"));
    UT_EXPECT_EQ(TimeFormatString((uint64_t)3661), std::string("1970-01-01 01:01:01"));
}

UT_TEST(TimeFormatStringUsesGivenTm)
{
    time_t sec = 0;
    struct tm tmVal;
    gmtime_r(&sec, &tmVal);

    UT_EXPECT_EQ(TimeFormatString(&tmVal), std::string("1970-01-01 00:00:00"));
    UT_EXPECT_EQ(TimeFormatString(&tmVal, 123, UT_TIME_MILLISEC_FORMAT_STR),
        std::string("1970-01-01 00:00:00.123"));
}

UT_TEST(PreciseFormatStringKeepsFraction)
{
    UT_EXPECT_EQ(TimeMicrosecondFormatString(1000123),
        std::string("1970-01-01 00:00:01.000123"));
    UT_EXPECT_EQ(TimeMillisecondFormatString(1123),
        std::string("1970-01-01 00:00:01.123"));
}

UT_TEST(CurrentTimeStringsHaveExpectedLength)
{
    UT_EXPECT_EQ(GetTimeString().size(), (size_t)19);
    UT_EXPECT_EQ(GetTimeMillisecondString().size(), (size_t)23);
    UT_EXPECT_EQ(GetTimeMicrosecondString().size(), (size_t)26);
}

UT_TEST(TimerMeasuresElapsedMicroseconds)
{
    Timer timer;
    timer.Start();
    MicroSleep(20000);
    uint64_t elapsed = timer.Stop();

    UT_EXPECT_TRUE(elapsed >= 10000);

    timer.Restart();
    UT_EXPECT_TRUE(timer.Stop() < elapsed);
}

UT_TEST_MAIN()
