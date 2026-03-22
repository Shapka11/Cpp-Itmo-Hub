#pragma once

#include <utility>

#include "Pipeline.h"


template<typename Predicate>
class Filter {
public:
    Filter(Predicate pred) : pred_(std::move(pred)) {}

    auto operator()(auto&& pipeline) {
        using IteratorType = decltype(pipeline.begin());
        return Pipeline(
            FilterIterator<IteratorType>(pipeline.begin(), pipeline.end(), pred_),
            FilterIterator<IteratorType>(pipeline.end(), pipeline.end(), pred_)
        );
    }

private:
    Predicate pred_;

    template <typename Iterator>
    class FilterIterator {
    public:
        FilterIterator(Iterator begin, Iterator end, const Predicate& pred) 
                : current_(std::move(begin))
                , end_(std::move(end))
                , pred_(pred) {
            AdvanceToNextMatch();
        }
        
        auto operator*() {
            return *current_; 
        }
        
        FilterIterator& operator++() { 
            ++current_; 
            AdvanceToNextMatch();
            return *this;
        }

        bool operator==(const FilterIterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const FilterIterator& other) const {
            return current_ != other.current_;
        }
        
    private:
        void AdvanceToNextMatch() {
            while (current_ != end_ && !pred_(*current_)) {
                ++current_;
            }
        }
        
        Iterator current_;
        Iterator end_;
        Predicate pred_;

    };

};
