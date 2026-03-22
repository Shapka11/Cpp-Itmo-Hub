#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(TransformTest, PowerOfTwo) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 4, 9, 16, 25));
}

TEST(TransformTest, FromStringToInt) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | Transform([](const std::string& str) { return std::stoi(str); }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
}

TEST(TransformTest, CustomTypeTransformation) {
    struct Point {
        int x, y;
        bool operator==(const Point& other) const = default;
    };

    std::vector<Point> points = {{1,2}, {3,4}, {5,6}};
    auto result = AsDataFlow(points)
        | Transform([](const Point& p) { return p.x + p.y; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(3, 7, 11));
}

TEST(TransformTest, MultipleTransformations) {
    std::vector<int> input = {1, 2, 3, 4};
    auto result = AsDataFlow(input)
        | Transform([](int x) { return x + 10; })
        | Transform([](int x) { return x * 2; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(22, 24, 26, 28));
}

TEST(TransformTest, EmptyInput) {
    std::vector<int> empty;
    auto result = AsDataFlow(empty)
        | Transform([](int x) { return x * 2; })
        | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(TransformTest, TransformAfterFilter) {
    std::vector<int> input = {1, 2, 3, 4, 5, 6};
    auto result = AsDataFlow(input)
        | Filter([](int x) { return x % 2 == 0; })
        | Transform([](int x) { return x * 10; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(20, 40, 60));
}

TEST(TransformTest, TransformWithStringStream) {
    std::vector<std::stringstream> streams(2);
    streams[0] << "10 20 30";
    streams[1] << "40 50 60";
    
    auto result = AsDataFlow(streams)
        | Split(" ")
        | Transform([](const std::string& s) { return std::stoi(s); })
        | AsVector();
    
    ASSERT_THAT(result, testing::ElementsAre(10, 20, 30, 40, 50, 60));
}
