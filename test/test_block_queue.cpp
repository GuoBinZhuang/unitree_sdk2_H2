#include <unitree/common/block_queue.hpp>
#include <unitree/common/time/sleep.hpp>

#include <thread>

#include "ut_test.hpp"

using namespace unitree::common;

UT_TEST(NewQueueIsEmpty)
{
    BlockQueue<int32_t> queue(4);

    UT_EXPECT_TRUE(queue.Empty());
    UT_EXPECT_EQ(queue.Size(), (uint64_t)0);
}

UT_TEST(PutAndGetPreserveFifoOrder)
{
    BlockQueue<int32_t> queue(4);

    UT_EXPECT_TRUE(queue.Put(1));
    UT_EXPECT_TRUE(queue.Put(2));
    UT_EXPECT_EQ(queue.Size(), (uint64_t)2);
    UT_EXPECT_FALSE(queue.Empty());

    int32_t value = 0;
    UT_EXPECT_TRUE(queue.Get(value, 1000));
    UT_EXPECT_EQ(value, 1);
    UT_EXPECT_TRUE(queue.Get(value, 1000));
    UT_EXPECT_EQ(value, 2);
    UT_EXPECT_TRUE(queue.Empty());
}

UT_TEST(PutFrontInsertsAtHead)
{
    BlockQueue<int32_t> queue(4);

    queue.Put(1);
    queue.Put(2, false, true);

    UT_EXPECT_EQ(queue.Get(1000), 2);
    UT_EXPECT_EQ(queue.Get(1000), 1);
}

UT_TEST(PutOnFullQueueFailsWithoutReplace)
{
    BlockQueue<int32_t> queue(2);

    UT_EXPECT_TRUE(queue.Put(1));
    UT_EXPECT_TRUE(queue.Put(2));
    UT_EXPECT_FALSE(queue.Put(3));
    UT_EXPECT_EQ(queue.Size(), (uint64_t)2);
    UT_EXPECT_EQ(queue.Get(1000), 1);
}

UT_TEST(PutOnFullQueueDropsOldestWithReplace)
{
    BlockQueue<int32_t> queue(2);

    queue.Put(1);
    queue.Put(2);

    UT_EXPECT_FALSE(queue.Put(3, true));
    UT_EXPECT_EQ(queue.Size(), (uint64_t)2);
    UT_EXPECT_EQ(queue.Get(1000), 2);
    UT_EXPECT_EQ(queue.Get(1000), 3);
}

UT_TEST(ZeroMaxSizeFallsBackToDefaultCapacity)
{
    BlockQueue<int32_t> queue(0);

    UT_EXPECT_TRUE(queue.Put(1));
    UT_EXPECT_TRUE(queue.Put(2));
    UT_EXPECT_EQ(queue.Size(), (uint64_t)2);
}

UT_TEST(GetOnEmptyQueueTimesOut)
{
    BlockQueue<int32_t> queue(4);

    int32_t value = -1;
    UT_EXPECT_FALSE(queue.Get(value, 10000));
    UT_EXPECT_EQ(value, -1);
}

UT_TEST(GetByValueThrowsOnTimeout)
{
    BlockQueue<int32_t> queue(4);

    UT_EXPECT_THROW(queue.Get(10000), TimeoutException);
}

UT_TEST(GetWakesUpWhenProducerPuts)
{
    BlockQueue<int32_t> queue(4);

    std::thread producer([&queue]()
    {
        MicroSleep(20000);
        queue.Put(7);
    });

    int32_t value = 0;
    UT_EXPECT_TRUE(queue.Get(value, 2000000));
    UT_EXPECT_EQ(value, 7);

    producer.join();
}

UT_TEST(InterruptUnblocksWaitingConsumer)
{
    BlockQueue<int32_t> queue(4);

    std::thread interrupter([&queue]()
    {
        MicroSleep(20000);
        queue.Interrupt(true);
    });

    int32_t value = 0;
    UT_EXPECT_FALSE(queue.Get(value, 2000000));

    interrupter.join();
}

UT_TEST(QueueSupportsNonTrivialElements)
{
    BlockQueue<std::string> queue(2);

    queue.Put("hello");

    std::string value;
    UT_EXPECT_TRUE(queue.Get(value, 1000));
    UT_EXPECT_EQ(value, std::string("hello"));
}

UT_TEST_MAIN()
