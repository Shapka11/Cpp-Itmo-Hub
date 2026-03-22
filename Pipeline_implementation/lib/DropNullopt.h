#pragma once

#include <utility>

#include "Pipeline.h"

class DropNullopt {
public:
    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());
        return Pipeline (
            DropNulloptIterator<IteratorType>(pipeline.begin(), pipeline.end()),
            DropNulloptIterator<IteratorType>(pipeline.end(), pipeline.end())
        );
    }

private:

    template<typename Iterator>
    class DropNulloptIterator {
    public:
        DropNulloptIterator(Iterator begin, Iterator end)
                : current_(std::move(begin))
                , end_(std::move(end)) {
            SkipNullopt();
        }

        DropNulloptIterator operator++() {
            ++current_;
            SkipNullopt();
            return *this;
        }

        auto operator*() {
            return **current_;
        }

        bool operator==(const DropNulloptIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const DropNulloptIterator& other) const {
            return current_ != other.current_;
        }

    private:
        Iterator current_;
        Iterator end_;

        void SkipNullopt() {
            while (current_ != end_ && !current_->has_value()) {
                ++current_;
            }
        }
    
    };

};
    