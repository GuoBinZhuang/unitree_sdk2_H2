#include <unitree/dds_wrapper/common/unitree_joystick.hpp>

#include <cstring>
#include <thread>

#include "ut_test.hpp"

using namespace unitree::common;

namespace
{
REMOTE_DATA_RX MakeKey()
{
    REMOTE_DATA_RX key;
    std::memset(&key, 0, sizeof(key));
    return key;
}

/*
 * Feeds an axis until its internal smoothed value converges to the input.
 */
void SettleAxis(Axis& axis, float value, int32_t times = 500)
{
    for (int32_t i = 0; i < times; i++)
    {
        axis(value);
    }
}
}

UT_TEST(ButtonUnionSharesBitsWithValue)
{
    BtnUnion btn;
    btn.value = 0;

    btn.components.R1 = 1;
    UT_EXPECT_EQ(btn.value, (uint16_t)0x0001);

    btn.value = 0;
    btn.components.left = 1;
    UT_EXPECT_EQ(btn.value, (uint16_t)0x8000);

    btn.value = 0x0100;
    UT_EXPECT_EQ((int32_t)btn.components.A, 1);
}

UT_TEST(ButtonTracksPressAndRelease)
{
    Button<int32_t> button;

    button(0);
    UT_EXPECT_FALSE(button.pressed);
    UT_EXPECT_FALSE(button.on_pressed);
    UT_EXPECT_FALSE(button.on_released);

    button(1);
    UT_EXPECT_TRUE(button.pressed);
    UT_EXPECT_TRUE(button.on_pressed);
    UT_EXPECT_FALSE(button.on_released);
    UT_EXPECT_EQ(button(), 1);

    button(1);
    UT_EXPECT_TRUE(button.pressed);
    UT_EXPECT_FALSE(button.on_pressed);

    button(0);
    UT_EXPECT_FALSE(button.pressed);
    UT_EXPECT_TRUE(button.on_released);
    UT_EXPECT_EQ(button(), 0);
}

UT_TEST(ButtonCountsFastClicksAndResetsSlowOnes)
{
    Button<int32_t> button;

    button(1);
    UT_EXPECT_EQ(button.click_cnt, 1);

    button(0);
    button(1);
    UT_EXPECT_EQ(button.click_cnt, 2);

    button(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    button(1);
    UT_EXPECT_EQ(button.click_cnt, 1);
}

UT_TEST(ButtonPressedTimeGrowsWhileHeldAndResetsAfterRelease)
{
    Button<int32_t> button;

    button(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    button(1);

    UT_EXPECT_TRUE(button.pressed_time >= 0.02f);

    button(0);
    button(0);
    UT_EXPECT_EQ(button.pressed_time, 0.0f);
}

UT_TEST(ButtonWorksWithNonIntegerPayload)
{
    Button<std::string> button;

    button("");
    UT_EXPECT_FALSE(button.pressed);

    button("a");
    UT_EXPECT_TRUE(button.pressed);
    UT_EXPECT_EQ(button(), std::string("a"));
}

UT_TEST(AxisSmoothsTowardsInput)
{
    Axis axis;

    axis(1.0f);
    UT_EXPECT_NEAR(axis(), axis.smooth, 1e-6);
    UT_EXPECT_TRUE(axis() < 1.0f);

    SettleAxis(axis, 1.0f);
    UT_EXPECT_NEAR(axis(), 1.0, 1e-3);
}

UT_TEST(AxisAppliesDeadzone)
{
    Axis axis;

    for (int32_t i = 0; i < 100; i++)
    {
        axis(0.005f);
    }

    UT_EXPECT_NEAR(axis(), 0.0, 1e-6);
}

UT_TEST(AxisBecomesPressedAboveThreshold)
{
    Axis axis;

    SettleAxis(axis, 0.4f);
    UT_EXPECT_FALSE(axis.pressed);

    SettleAxis(axis, 1.0f);
    UT_EXPECT_TRUE(axis.pressed);
    UT_EXPECT_TRUE(axis.click_cnt >= 1);

    SettleAxis(axis, 0.0f);
    UT_EXPECT_FALSE(axis.pressed);
}

UT_TEST(ExtractMapsButtonBitsToNamedKeys)
{
    REMOTE_DATA_RX key = MakeKey();
    key.RF_RX.btn.components.Select = 1;
    key.RF_RX.btn.components.Start = 1;
    key.RF_RX.btn.components.L1 = 1;
    key.RF_RX.btn.components.R1 = 1;
    key.RF_RX.btn.components.f1 = 1;
    key.RF_RX.btn.components.f2 = 1;
    key.RF_RX.btn.components.A = 1;
    key.RF_RX.btn.components.up = 1;

    UnitreeJoystick joystick;
    joystick.extract(key);

    UT_EXPECT_TRUE(joystick.back.pressed);
    UT_EXPECT_TRUE(joystick.start.pressed);
    UT_EXPECT_TRUE(joystick.LB.pressed);
    UT_EXPECT_TRUE(joystick.RB.pressed);
    UT_EXPECT_TRUE(joystick.F1.pressed);
    UT_EXPECT_TRUE(joystick.F2.pressed);
    UT_EXPECT_TRUE(joystick.A.pressed);
    UT_EXPECT_TRUE(joystick.up.pressed);

    UT_EXPECT_FALSE(joystick.B.pressed);
    UT_EXPECT_FALSE(joystick.X.pressed);
    UT_EXPECT_FALSE(joystick.Y.pressed);
    UT_EXPECT_FALSE(joystick.down.pressed);
    UT_EXPECT_FALSE(joystick.left.pressed);
    UT_EXPECT_FALSE(joystick.right.pressed);
}

UT_TEST(ExtractFeedsAxesWithSmoothing)
{
    REMOTE_DATA_RX key = MakeKey();
    key.RF_RX.lx = 1.0f;
    key.RF_RX.ly = -1.0f;
    key.RF_RX.rx = 0.5f;
    key.RF_RX.ry = -0.5f;

    UnitreeJoystick joystick;

    for (int32_t i = 0; i < 500; i++)
    {
        joystick.extract(key);
    }

    UT_EXPECT_NEAR(joystick.lx(), 1.0, 1e-3);
    UT_EXPECT_NEAR(joystick.ly(), -1.0, 1e-3);
    UT_EXPECT_NEAR(joystick.rx(), 0.5, 1e-3);
    UT_EXPECT_NEAR(joystick.ry(), -0.5, 1e-3);
}

UT_TEST(CombineRebuildsRemoteData)
{
    REMOTE_DATA_RX key = MakeKey();
    key.RF_RX.btn.components.B = 1;
    key.RF_RX.btn.components.right = 1;
    key.RF_RX.btn.components.L2 = 1;
    key.RF_RX.lx = 1.0f;
    key.RF_RX.ry = -1.0f;

    UnitreeJoystick joystick;

    for (int32_t i = 0; i < 500; i++)
    {
        joystick.extract(key);
    }

    REMOTE_DATA_RX combined = joystick.combine();

    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.B, 1);
    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.right, 1);
    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.A, 0);
    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.L2, 1);
    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.R2, 0);
    UT_EXPECT_NEAR(combined.RF_RX.lx, 1.0, 1e-3);
    UT_EXPECT_NEAR(combined.RF_RX.ry, -1.0, 1e-3);
}

UT_TEST(CombineThresholdsTriggerAxesIntoButtons)
{
    UnitreeJoystick joystick;

    SettleAxis(joystick.LT, 0.4f);
    SettleAxis(joystick.RT, 0.9f);

    REMOTE_DATA_RX combined = joystick.combine();

    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.L2, 0);
    UT_EXPECT_EQ((int32_t)combined.RF_RX.btn.components.R2, 1);
}

UT_TEST(RemoteDataLayoutMatchesWireBuffer)
{
    UT_EXPECT_EQ(sizeof(REMOTE_DATA_RX), (size_t)40);
    UT_EXPECT_EQ(offsetof(BtnDataStruct, btn), (size_t)2);
    UT_EXPECT_EQ(offsetof(BtnDataStruct, lx), (size_t)4);
    UT_EXPECT_EQ(offsetof(BtnDataStruct, ly), (size_t)20);
}

UT_TEST_MAIN()
