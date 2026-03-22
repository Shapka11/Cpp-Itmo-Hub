#include <processing.h>
#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(WriteTest, Write) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::stringstream file_emulator;
    auto result = AsDataFlow(input) | Write(file_emulator, '|');
    ASSERT_EQ(file_emulator.str(), "1|2|3|4|5|");
}

TEST(WriteTest, EmptyInput) {
    std::vector<int> input;
    std::stringstream ss;
    AsDataFlow(input) | Write(ss, ',');
    EXPECT_TRUE(ss.str().empty());
}

TEST(WriteTest, CustomTypes) {
    struct Point { int x, y; };
    std::vector<Point> input = {{1,2}, {3,4}};
    std::stringstream ss;
    
    AsDataFlow(input) 
        | Transform([](const Point& p) { return std::format("({},{})", p.x, p.y); })
        | Write(ss, ';');
    
    EXPECT_EQ(ss.str(), "(1,2);(3,4);");
}
