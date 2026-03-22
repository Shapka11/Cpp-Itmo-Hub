#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(FlowConvertionsTest, AsDataFlow) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto flow = AsDataFlow(input);
    ASSERT_THAT(flow, testing::ElementsAre(1, 2, 3, 4, 5));
}

TEST(FlowConvertionsTest, AsVector) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | AsVector();
    ASSERT_THAT(result, testing::ElementsAreArray(std::vector<int>{1, 2, 3, 4, 5}));
}

TEST(FlowConversionsTest, AsDataFlow_StringVector) {
    std::vector<std::string> input = {"a", "bb", "ccc"};
    auto flow = AsDataFlow(input);
    ASSERT_THAT(flow, testing::ElementsAre("a", "bb", "ccc"));
}

TEST(FlowConversionsTest, AsVector_EmptyFlow) {
    auto flow = AsDataFlow(std::vector<int>{});
    auto result = flow | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(FlowConversionsTest, AsVector_AfterTransform) {
    auto result = AsDataFlow(std::vector<int>{1, 2, 3})
                 | Transform([](int x) { return x * 2; })
                 | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 4, 6));
}

TEST(FlowConversionsTest, AsVector_AfterFilter) {
    auto result = AsDataFlow(std::vector<int>{1, 2, 3, 4, 5})
                 | Filter([](int x) { return x % 2 == 0; })
                 | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(FlowConversionsTest, AsVector_MultiplePipes) {
    auto result = AsDataFlow(std::vector<int>{1, 2, 3, 4, 5, 6})
                 | Filter([](int x) { return x > 2; })
                 | Transform([](int x) { return x * 10; })
                 | Filter([](int x) { return x < 60; })
                 | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(30, 40, 50));
}

TEST(FlowConversionsTest, AsVector_PreservesOrder) {
    std::vector<int> input = {5, 3, 1, 4, 2};
    auto result = AsDataFlow(input) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(5, 3, 1, 4, 2));
}

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

TEST(FlowConversionsTest, AsVector_CustomType) {
    std::vector<Point> input = {{1,2}, {3,4}, {5,6}};
    auto result = AsDataFlow(input) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(Point{1,2}, Point{3,4}, Point{5,6}));
}
