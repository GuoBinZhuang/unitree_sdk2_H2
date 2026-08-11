#include <unitree/common/json/jsonize.hpp>

#include "ut_test.hpp"

using namespace unitree::common;

namespace
{
class Point : public Jsonize
{
public:
    Point() : x(0), y(0)
    {}

    Point(int32_t xVal, int32_t yVal) : x(xVal), y(yVal)
    {}

    void fromJson(JsonMap& json)
    {
        FromJson(json["x"], x);
        FromJson(json["y"], y);
    }

    void toJson(JsonMap& json) const
    {
        ToJson(x, json["x"]);
        ToJson(y, json["y"]);
    }

public:
    int32_t x;
    int32_t y;
};
}

UT_TEST(FromJsonStringParsesScalarTypes)
{
    Any a = FromJsonString("{\"i\":1,\"d\":1.5,\"b\":true,\"s\":\"abc\",\"n\":null}");

    UT_EXPECT_TRUE(IsJsonMap(a));
    UT_EXPECT_TRUE(IsJsonObject(a));

    const JsonMap& m = AnyCast<JsonMap>(a);

    UT_EXPECT_EQ(AnyNumberCast<int64_t>(m.at("i")), (int64_t)1);
    UT_EXPECT_NEAR(AnyNumberCast<double>(m.at("d")), 1.5, 1e-9);
    UT_EXPECT_EQ(AnyNumberCast<bool>(m.at("b")), true);
    UT_EXPECT_EQ(AnyCast<std::string>(m.at("s")), std::string("abc"));
    UT_EXPECT_TRUE(IsNull(m.at("n")));
}

UT_TEST(FromJsonStringParsesArray)
{
    Any a = FromJsonString("[1,2,3]");

    UT_EXPECT_TRUE(IsJsonArray(a));

    const JsonArray& arr = AnyCast<JsonArray>(a);

    UT_EXPECT_EQ(arr.size(), (size_t)3);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(arr[2]), 3);
}

UT_TEST(FromJsonStringParsesNestedStructures)
{
    Any a = FromJsonString("{\"outer\":{\"inner\":[{\"k\":\"v\"}]}}");

    const JsonMap& outer = AnyCast<JsonMap>(AnyCast<JsonMap>(a).at("outer"));
    const JsonArray& inner = AnyCast<JsonArray>(outer.at("inner"));
    const JsonMap& first = AnyCast<JsonMap>(inner.at(0));

    UT_EXPECT_EQ(AnyCast<std::string>(first.at("k")), std::string("v"));
}

UT_TEST(FromJsonStringRejectsMalformedJson)
{
    UT_EXPECT_THROW(FromJsonString("{\"a\":}"), Exception);
    UT_EXPECT_THROW(FromJsonString("[1,2"), Exception);
}

UT_TEST(ToJsonStringSerializesMapAndArray)
{
    JsonMap m;
    m["a"] = (int32_t)1;
    m["b"] = std::string("x");

    UT_EXPECT_EQ(ToJsonString(Any(m)), std::string("{\"a\":1,\"b\":\"x\"}"));

    JsonArray arr;
    arr.push_back(Any((int32_t)1));
    arr.push_back(Any(std::string("x")));

    UT_EXPECT_EQ(ToJsonString(Any(arr)), std::string("[1,\"x\"]"));
}

UT_TEST(PrettyJsonStringContainsNewlines)
{
    JsonMap m;
    m["a"] = (int32_t)1;

    std::string pretty = ToJsonString(Any(m), true);

    UT_EXPECT_TRUE(pretty.find('\n') != std::string::npos);
    UT_EXPECT_TRUE(pretty.find("\"a\"") != std::string::npos);
}

UT_TEST(JsonStringRoundTripKeepsValues)
{
    std::string s = "{\"a\":1,\"b\":[1,2],\"c\":{\"d\":\"e\"}}";

    UT_EXPECT_EQ(ToJsonString(FromJsonString(s)), s);
}

UT_TEST(JsonizeObjectRoundTrips)
{
    Point point(1, 2);

    std::string s = ToJsonString(point);
    UT_EXPECT_TRUE(s.find("\"x\":1") != std::string::npos);
    UT_EXPECT_TRUE(s.find("\"y\":2") != std::string::npos);

    Point parsed;
    FromJsonString(s, parsed);

    UT_EXPECT_EQ(parsed.x, 1);
    UT_EXPECT_EQ(parsed.y, 2);
}

UT_TEST(JsonizeContainersRoundTrip)
{
    std::vector<Point> points;
    points.push_back(Point(1, 2));
    points.push_back(Point(3, 4));

    std::vector<Point> parsed;
    FromJsonString(ToJsonString(points), parsed);

    UT_EXPECT_EQ(parsed.size(), (size_t)2);
    UT_EXPECT_EQ(parsed[1].x, 3);
    UT_EXPECT_EQ(parsed[1].y, 4);

    std::map<std::string,std::string> map;
    map["k1"] = "v1";
    map["k2"] = "v2";

    std::map<std::string,std::string> parsedMap;
    FromJsonString(ToJsonString(map), parsedMap);

    UT_EXPECT_EQ(parsedMap.size(), (size_t)2);
    UT_EXPECT_EQ(parsedMap["k2"], std::string("v2"));
}

UT_TEST(FromJsonOnEmptyAnyLeavesContainerEmpty)
{
    std::vector<int32_t> vec;
    FromJson(UT_EMPTY_ANY, vec);

    UT_EXPECT_TRUE(vec.empty());

    std::list<int32_t> list;
    FromJson(UT_EMPTY_ANY, list);
    UT_EXPECT_TRUE(list.empty());

    std::set<int32_t> set;
    FromJson(UT_EMPTY_ANY, set);
    UT_EXPECT_TRUE(set.empty());
}

UT_TEST(JsonizeMacrosReadAndWriteFields)
{
    JsonMap m;
    int32_t value = 3;

    JN_TO(m, "value", value);
    UT_EXPECT_EQ(AnyNumberCast<int32_t>(m["value"]), 3);

    int32_t parsed = 0;
    JN_FROM(m, "value", parsed);
    UT_EXPECT_EQ(parsed, 3);

    int32_t missing = 9;
    JN_FROM_WEAK(m, "absent", missing);
    UT_EXPECT_EQ(missing, 9);
}

UT_TEST_MAIN()
