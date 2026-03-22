#pragma once

#include <fstream>
#include <istream>
#include <memory>
#include <utility>

#include "Pipeline.h"


class OpenFiles {
public:
    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());
        return Pipeline(
            FileIterator<IteratorType>(pipeline.begin(), pipeline.end()),
            FileIterator<IteratorType>(pipeline.end(), pipeline.end())
        );
    }

private:
    template<typename Iterator>
    class FileIterator {
    public:
        FileIterator(Iterator begin, Iterator end)
                : current_(std::move(begin)), end_(std::move(end)) {
            if (current_ != end_) {
                OpenNextFile();
            }
        }

        std::istream& operator*() {
            return *stream_;
        }

        std::istream* operator->() {
            return stream_.get();
        }

        FileIterator& operator++() {
            ++current_;
            if (current_ != end_) {
                OpenNextFile();
            }
            return *this;
        }

        bool operator==(const FileIterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const FileIterator& other) const {
            return current_ != other.current_;
        }

    private:
        void OpenNextFile() {
            stream_.reset();
            
            while (current_ != end_) {
                const auto& entry = *current_;
                stream_ = std::make_shared<std::ifstream>(entry);
                
                if (stream_ && *stream_) {
                    break;
                }
                
                stream_.reset();
                ++current_;
            }
        }

        Iterator current_;
        Iterator end_;
        std::shared_ptr<std::ifstream> stream_;

    };

};

    