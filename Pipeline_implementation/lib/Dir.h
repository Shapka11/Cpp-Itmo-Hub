#pragma once

#include <filesystem>
#include <variant>
#include <utility>

#include "Pipeline.h"


class Dir {
private:
    const char* path_;
    bool is_recursive_;

    class DirIterator {
    public:
        using FileIterator = std::variant<
            std::filesystem::directory_iterator,
            std::filesystem::recursive_directory_iterator
        >;

        DirIterator() = default;
        DirIterator(FileIterator iterator) : iterator_(std::move(iterator)) {
            UpdatePath();
        }

        std::filesystem::path& operator*() const {
            return path_;
        }

        DirIterator& operator++() {
            std::visit([](auto&& it) { ++it; }, iterator_);
            UpdatePath();
            return *this;
        }

        bool operator==(const DirIterator& other) const {
            return iterator_ == other.iterator_;
        }
    
        bool operator!=(const DirIterator& other) const {
            return iterator_ != other.iterator_;
        }

    private:
        FileIterator iterator_;
        mutable std::filesystem::path path_;

        void UpdatePath() {
            std::visit([this](const auto& it) {
                using IteratorType = std::decay_t<decltype(it)>;
                if constexpr (std::is_same_v<IteratorType, std::filesystem::directory_iterator>) {
                    path_ = (it == std::filesystem::directory_iterator{}) ? "" : it->path();
                }
                else if constexpr (std::is_same_v<IteratorType, std::filesystem::recursive_directory_iterator>) {
                    path_ = (it == std::filesystem::recursive_directory_iterator{}) ? "" : it->path();
                }
            }, iterator_);
        }

    };

public:
    Dir(const char* path, bool is_recursive = false) 
            : path_(path)
            , is_recursive_(is_recursive) {}  

    DirIterator begin() const {
        if (is_recursive_) {
            return DirIterator(std::filesystem::recursive_directory_iterator(path_));
        }
        return DirIterator(std::filesystem::directory_iterator(path_));
    }

    DirIterator end() const {
        if (is_recursive_) {
            return DirIterator(std::filesystem::recursive_directory_iterator{});
        }
        return DirIterator(std::filesystem::directory_iterator{});
    }

};
