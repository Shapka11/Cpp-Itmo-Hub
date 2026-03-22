#pragma once

#include <utility>

#include "Pipeline.h"


class SplitExpected {
public:
    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());
        return std::make_pair(
            Pipeline(
                UnexpectedIterator<IteratorType>(pipeline.begin(), pipeline.end()),
                UnexpectedIterator<IteratorType>(pipeline.end(), pipeline.end())
            ),
            Pipeline(
                ExpectedIterator<IteratorType>(pipeline.begin(), pipeline.end()),
                ExpectedIterator<IteratorType>(pipeline.end(), pipeline.end())
            )
        );
    }

private:  
    template<typename Iterator>
    class UnexpectedIterator {
    public:
        UnexpectedIterator(Iterator begin, Iterator end)
                : current_(std::move(begin))
                , end_(std::move(end)) {}

        auto operator*() {
            GoToNextValue();
            return (*current_).error();
        }

        UnexpectedIterator& operator++() {
            ++current_;
            GoToNextValue();
            return *this;
        }

        bool operator==(const UnexpectedIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const UnexpectedIterator& other) const {
            return current_ != other.current_;
        }

    private:
        void GoToNextValue() {
            while (current_ != end_ && (*current_).has_value()) {
                ++current_;
            }
        }

        Iterator current_;
        Iterator end_;
    };

    template<typename Iterator>
    class ExpectedIterator {
    public:
        ExpectedIterator(Iterator begin, Iterator end)
                : current_(std::move(begin))
                , end_(std::move(end)) {}


        auto operator*() {
            GoToNextValue();
            return (*current_).value();
        }

        ExpectedIterator& operator++() {
            ++current_;
            GoToNextValue();
            return *this;
        }

        bool operator==(const ExpectedIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const ExpectedIterator& other) const {
            return current_ != other.current_;
        }

    private:
        void GoToNextValue() {
            while (current_ != end_ && !(*current_).has_value()) {
                ++current_;
            }
        }

        Iterator current_;
        Iterator end_;

    };

};
    
    