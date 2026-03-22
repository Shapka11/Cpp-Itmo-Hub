#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <vector>


TEST(ReadTest, ByNewLine) {
    std::vector<std::stringstream> files(2);
    files[0] << "1\n2\n3\n4\n5";
    files[1] << "6\n7\n8\n9\n10";
    auto result = AsDataFlow(files) | Split("\n") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}

TEST(ReadTest, BySpace) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}

TEST(SplitTest, SplitAndTransform) {
    std::vector<std::stringstream> inputs{1};
    inputs[0] << "1 2 3 4 5";
    
    auto result = AsDataFlow(inputs) 
        | Split(" ") 
        | Transform([](const std::string& s) { return std::stoi(s) * 2; }) 
        | AsVector();
    
    ASSERT_THAT(result, testing::ElementsAre(2, 4, 6, 8, 10));
}

TEST(SplitTest, SplitWithMultipleDelimiters) {
    std::vector<std::stringstream> inputs{1};
    inputs[0] << "apple,banana;cherry|orange";
    
    auto result = AsDataFlow(inputs) | Split("[,;|]") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("apple", "banana", "cherry", "orange"));
}

TEST(SplitTest, SplitPreserveEmptyLines) {
    std::vector<std::stringstream> inputs{1};
    inputs[0] << "first\n\nthird\n\n\nsixth";
    
    auto result = AsDataFlow(inputs) | Split("\n") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("first", "", "third", "", "", "sixth"));
}
