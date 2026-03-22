#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <set>

// please indicate your path
const char* files_path = "/home/sasha/github/labwork8-Shapka11/tests/files";

TEST(DirTest, DirFilesNoRecursive) {
    auto files = Dir(files_path, false) | AsVector();
    
    std::set<std::string> files_str;
    for (const auto& file : files) {
        files_str.insert(file.string());
    }
    
    std::string path = files_path;
    std::set<std::string> expected {
        path + "/a.txt",
        path + "/b.jpg",
        path + "/c.txt",
        path + "/files_2"
    };
    
    ASSERT_THAT(files_str, testing::ContainerEq(expected));
}

TEST(DirTest, DirFilesRecursive) {
    auto files = Dir(files_path, true) | AsVector();
    
    std::set<std::string> files_str;
    for (const auto& file : files) {
        files_str.insert(file.string());
    }
    
    std::string path = files_path;
    std::set<std::string> expected {
        path + "/a.txt",
        path + "/b.jpg",
        path + "/c.txt",
        path + "/files_2",
        path + "/files_2/d.txt"
    };
    
    ASSERT_THAT(files_str, testing::ContainerEq(expected));
}

TEST(DirTest, DataFiles) {
    auto files = Dir(files_path, false) 
        | Filter([](std::filesystem::path& p){ return p.extension() == ".txt"; }) 
        | OpenFiles() 
        | Split(" ") 
        | AsVector();

    
    std::set<std::string> files_str;
    files_str.insert("Abcd");
    files_str.insert("abc");
    files_str.insert("ab");
    files_str.insert("A");
    files_str.insert("hello");
    files_str.insert("world");
    
    std::set<std::string> res;
    for (auto& i : files) {
        res.insert(i);
    }
    
    ASSERT_THAT(res, testing::ContainerEq(files_str));
}
