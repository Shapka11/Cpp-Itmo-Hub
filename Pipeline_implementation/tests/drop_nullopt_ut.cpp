#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(DropNulloptTest, DropNullopt) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 3, 5));
}

TEST(DropNulloptTest, EmptyInput) {
    std::vector<std::optional<int>> input = {};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(DropNulloptTest, AllNullopts) {
    std::vector<std::optional<int>> input = {std::nullopt, std::nullopt, std::nullopt};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_TRUE(result.empty());
}

TEST(DropNulloptTest, NoNullopts) {
    std::vector<std::optional<int>> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5));
}
