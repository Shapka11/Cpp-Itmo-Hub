#pragma once

#include <utility>


template <typename Iterator>
class Pipeline {
public:
    explicit Pipeline(Iterator&& begin, Iterator&& end) 
            : begin_(std::move(begin))
            , end_(std::move(end)) {}
    
    Iterator begin() { return std::move(begin_); }
    Iterator end() { return std::move(end_); }
    
private:
    Iterator begin_;
    Iterator end_;

};
