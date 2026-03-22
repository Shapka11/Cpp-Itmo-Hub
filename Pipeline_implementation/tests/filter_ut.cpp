#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(FilterTest, FilterEven) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(FilterTest, FilterUpperCase) {
    std::vector<std::string> input = {"hello", "world", "HELLO", "WORLD"};
    auto result =
        AsDataFlow(input)
            | Filter([](const std::string& x) { return std::all_of(x.begin(), x.end(), [](char c) { return std::isupper(c); }); })
            | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD"));
}

TEST(FilterTest, EmptyInput) {
    std::vector<int> input = {};
    auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(FilterTest, FilterGreaterThan) {
    std::vector<int> input = {5, 10, 15, 20, 25};
    auto result = AsDataFlow(input) | Filter([](int x) { return x > 15; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(20, 25));
}

TEST(FilterTest, FilterShortStrings) {
    std::vector<std::string> input = {"a", "ab", "abc", "abcd", "abcde"};
    auto result = AsDataFlow(input)
        | Filter([](const std::string& s) { return s.length() <= 3; })
        | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("a", "ab", "abc"));
}

TEST(FilterTest, FilterCustomStruct) {
    struct Person {
        std::string name;
        int age;
    };
    
    std::vector<Person> input = {
        {"A", 52},
        {"B", 17},
        {"C", 37},
        {"D", 10}
    };
    
    auto result = AsDataFlow(input)
        | Filter([](const Person& p) { return p.age >= 18; })
        | AsVector();
    
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0].name, "A");
    ASSERT_EQ(result[1].name, "C");
}
