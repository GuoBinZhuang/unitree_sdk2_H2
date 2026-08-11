#include <unitree/common/any.hpp>

#include "ut_test.hpp"

using namespace unitree::common;

UT_TEST(DefaultAnyIsEmpty)
{
    Any a;

    UT_EXPECT_TRUE(a.Empty());
    UT_EXPECT_TRUE(a.GetTypeInfo() == typeid(void));
}

UT_TEST(AnyKeepsValueType)
{
    UT_EXPECT_TRUE(IsInt(Any((int32_t)1)));
    UT_EXPECT_TRUE(IsUint(Any((uint32_t)1)));
    UT_EXPECT_TRUE(IsInt64(Any((int64_t)1)));
    UT_EXPECT_TRUE(IsUint64(Any((uint64_t)1)));
    UT_EXPECT_TRUE(IsInt8(Any((int8_t)1)));
    UT_EXPECT_TRUE(IsUint8(Any((uint8_t)1)));
    UT_EXPECT_TRUE(IsInt16(Any((int16_t)1)));
    UT_EXPECT_TRUE(IsUint16(Any((uint16_t)1)));
    UT_EXPECT_TRUE(IsFloat(Any(1.0f)));
    UT_EXPECT_TRUE(IsDouble(Any(1.0)));
    UT_EXPECT_TRUE(IsLongDouble(Any((long double)1.0)));
    UT_EXPECT_TRUE(IsBool(Any(true)));
    UT_EXPECT_TRUE(IsString(Any("abc")));
}

UT_TEST(CharPointerConstructorsBuildStrings)
{
    Any a("abcdef", 3);
    UT_EXPECT_TRUE(IsString(a));
    UT_EXPECT_EQ(AnyCast<std::string>(a), std::string("abc"));

    /*
     * ToString returns a reference into the Any, so both must outlive the check.
     */
    Any b("abc");
    UT_EXPECT_EQ(ToString(b), std::string("abc"));

    Any empty;
    UT_EXPECT_EQ(ToString(empty), std::string(""));
}

UT_TEST(CopyConstructorIsDeep)
{
    Any a((int32_t)5);
    Any b(a);

    UT_EXPECT_EQ(AnyCast<int32_t>(b), 5);
    UT_EXPECT_TRUE(a.mContent != b.mContent);
}

UT_TEST(AssignmentReplacesContent)
{
    Any a((int32_t)5);
    a = std::string("abc");

    UT_EXPECT_TRUE(IsString(a));
    UT_EXPECT_EQ(AnyCast<std::string>(a), std::string("abc"));

    Any b((int32_t)7);
    a = b;
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(a), 7);
}

UT_TEST(SwapExchangesContent)
{
    Any a((int32_t)1);
    Any b(std::string("abc"));

    a.Swap(b);

    UT_EXPECT_TRUE(IsString(a));
    UT_EXPECT_TRUE(IsInt(b));
}

UT_TEST(IsIntegerAndIsNumberClassifyTypes)
{
    UT_EXPECT_TRUE(IsInteger(Any((int16_t)1)));
    UT_EXPECT_FALSE(IsInteger(Any(1.0)));
    UT_EXPECT_TRUE(IsNumber(Any(1.0)));
    UT_EXPECT_TRUE(IsNumber(Any(true)));
    UT_EXPECT_FALSE(IsNumber(Any(std::string("1"))));
}

UT_TEST(TypePredicatesMatchTypeInfo)
{
    UT_EXPECT_TRUE(IsIntegerType(typeid(uint64_t)));
    UT_EXPECT_FALSE(IsIntegerType(typeid(float)));
    UT_EXPECT_TRUE(IsNumberType(typeid(long double)));
    UT_EXPECT_FALSE(IsNumberType(typeid(std::string)));
    UT_EXPECT_TRUE(IsTypeEqual(typeid(int32_t), typeid(int32_t)));
    UT_EXPECT_FALSE(IsTypeEqual(typeid(int32_t), typeid(uint32_t)));
}

UT_TEST(AnyCastRequiresExactType)
{
    Any a((int32_t)5);

    UT_EXPECT_EQ(AnyCast<int32_t>(a), 5);
    UT_EXPECT_THROW(AnyCast<int64_t>(a), BadCastException);
    UT_EXPECT_THROW(AnyCast<std::string>(a), BadCastException);
}

UT_TEST(AnyNumberCastConvertsBetweenNumberTypes)
{
    UT_EXPECT_EQ(AnyNumberCast<int64_t>(Any((int32_t)-5)), (int64_t)-5);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((uint8_t)200)), 200);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((int16_t)-7)), -7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((uint16_t)7)), 7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((uint32_t)7)), 7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((int64_t)7)), 7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((uint64_t)7)), 7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any((int8_t)-7)), -7);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(Any(2.75)), 2);
    UT_EXPECT_NEAR(AnyNumberCast<double>(Any(1.5f)), 1.5, 1e-6);
    UT_EXPECT_NEAR(AnyNumberCast<double>(Any((long double)1.5)), 1.5, 1e-6);
}

UT_TEST(AnyNumberCastRejectsNonNumbers)
{
    UT_EXPECT_THROW(AnyNumberCast<int32_t>(Any(std::string("1"))), BadCastException);
    UT_EXPECT_THROW(AnyNumberCast<int32_t>(Any()), BadCastException);
}

UT_TEST(StringToAnyStoresString)
{
    Any a;

    StringTo("abc", a);
    UT_EXPECT_EQ(AnyCast<std::string>(a), std::string("abc"));

    StringTo("abcdef", (size_t)3, a);
    UT_EXPECT_EQ(AnyCast<std::string>(a), std::string("abc"));

    StringTo(std::string("xyz"), a);
    UT_EXPECT_EQ(AnyCast<std::string>(a), std::string("xyz"));
}

UT_TEST_MAIN()
