#pragma once

#include <algorithm>
#include <format>
#include <utility>

#include "Pipeline.h"


template<typename Predicate>
class Transform {
public:
    Transform(Predicate transformer) : transformer_(transformer) {}

    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());
        return Pipeline(
            TransformIterator<IteratorType>(pipeline.begin(), pipeline.end(), transformer_),
            TransformIterator<IteratorType>(pipeline.end(), pipeline.end(), transformer_)
        );
    }

private:
    Predicate transformer_;

    template<typename Iterator>
    class TransformIterator {
    public:
        TransformIterator(Iterator begin, Iterator end, Predicate pred) 
                : current_(std::move(begin))
                , end_(std::move(end))
                , predicate_(std::move(pred)) {}

        TransformIterator& operator++() {
            ++current_;
            return *this;
        }

        auto operator*() {
            auto&& value = *current_;
            return predicate_(value);
        }

        bool operator==(const TransformIterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const TransformIterator& other) const {
            return current_ != other.current_;
        } 

    private:
        Iterator current_;
        Iterator end_;
        Predicate predicate_;

    };

};
