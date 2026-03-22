#pragma once

#include <istream>
#include <string>
#include <utility>

#include "Pipeline.h"


class Split {
public:
    Split(const std::string& delimiters) : delimiters_(std::move(delimiters)) {}

    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());

        return Pipeline(
            SplitIterator<IteratorType>(pipeline.begin(), pipeline.end(), delimiters_),
            SplitIterator<IteratorType>(pipeline.end(), pipeline.end(), delimiters_)
        );
    }
    
private:
    std::string delimiters_;
    
    template <typename Iterator>
    class SplitIterator {
    public:
        SplitIterator(Iterator begin, Iterator end, const std::string& delimiters)
                : current_(std::move(begin))
                , end_(std::move(end))
                , delimiters_(delimiters) {}

        auto operator*() {
            if (!is_initialize_) {
                ResetStream();
                AdvanceToNextToken();
                is_initialize_ = true;
            }
            return current_word_;
        }

        SplitIterator& operator++() {
            AdvanceToNextToken();
            return *this;
        }

        bool operator==(const SplitIterator& other) const {
            return current_ == other.current_ && current_word_ == other.current_word_;
        }

        bool operator!=(const SplitIterator& other) const {
            return !(*this == other);
        }
    
    private:
        Iterator current_;
        Iterator end_;
        std::istream* current_stream_ = nullptr;
        const std::string& delimiters_;
        
        std::string current_word_;
        char current_char_ = EOF;
        bool is_initialize_ = false;

        void ResetStream() {
            if (current_ != end_) {
                current_stream_ = dynamic_cast<std::istream*>(&(*current_));
                if (current_stream_) {
                    current_stream_->clear();
                    current_stream_->seekg(0, std::ios::beg);
                }
            }
        }

        void NextStream() {
            ++current_;
            if (current_ == end_) {
                current_stream_ = nullptr;
                return;
            }

            ResetStream();
            return;
        }

        void AdvanceToNextToken() {
            current_word_.clear();
            
            bool is_word_empty = true;

            while (current_ != end_) {
                if (!current_stream_) {
                    NextStream();
                    if (!current_stream_) {
                        return;
                    } 
                }

                if (!current_stream_->get(current_char_)) {
                    if (!is_word_empty || !current_word_.empty()) {
                        return;
                    }
                    NextStream();
                    if (!current_stream_) {
                        return;
                    }
                    continue;
                }

                if (delimiters_.find(current_char_) != std::string::npos) {
                    return;
                }

                is_word_empty = false;
                current_word_ += current_char_;

            }
        }
    
    };

};
    