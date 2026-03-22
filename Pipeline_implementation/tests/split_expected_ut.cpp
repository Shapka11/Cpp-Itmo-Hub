#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <expected>


struct Department {
    std::string name;
    bool operator==(const Department& other) const = default;
};

struct User {
    std::string username;
    int age;
    bool operator==(const User& other) const = default;
};

std::expected<User, std::string> ParseUser(const std::string& input) {
    auto pos = input.find(',');
    if (pos == std::string::npos) {
        return std::unexpected("Invalid format: missing comma");
    }
    
    try {
        return User{
            input.substr(0, pos),
            std::stoi(input.substr(pos + 1))
        };
    } catch (...) {
        return std::unexpected("Invalid age format");
    }
}

std::expected<Department, std::string> ParseDepartment(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}

TEST(SplitExpectedTest, SplitExpected) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department||another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartment) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}

TEST(SplitExpectedTest, EmptyInput) {
    std::vector<std::string> inputs;

    auto [errors, valid_users] = AsDataFlow(inputs)
        | Transform(ParseUser)
        | SplitExpected();

    ASSERT_TRUE((errors | AsVector()).empty());
    ASSERT_TRUE((valid_users | AsVector()).empty());
}

TEST(SplitExpectedTest, WriteErrorsToStream) {
    std::vector<std::string> inputs = {
        "a,25",
        "invalid",
        "b,30",
        "c,invalid"
    };

    auto [errors, _] = AsDataFlow(inputs)
        | Transform(ParseUser)
        | SplitExpected();

    std::stringstream error_stream;
    errors | Write(error_stream, '\n');

    std::string output = error_stream.str();
    ASSERT_TRUE(output.find("Invalid format: missing comma") != std::string::npos);
    ASSERT_TRUE(output.find("Invalid age format") != std::string::npos);
}
