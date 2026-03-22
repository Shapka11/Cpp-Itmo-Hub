#pragma once

#include <iterator>
#include <utility>

#include "Pipeline.h"


template<typename Container>
class AsDataFlow {
public:
    using iterator = decltype(std::begin(std::declval<Container&>()));
    using const_iterator = decltype(std::begin(std::declval<const Container&>()));
    using value_type = typename std::iterator_traits<iterator>::value_type;

    AsDataFlow(Container& container)
            : begin_(std::begin(container)), end_(std::end(container)) {}

    AsDataFlow(Container&& container) 
            : begin_(std::begin(container)), end_(std::end(container)) {}


    auto begin() const {
        return begin_;
    }

    auto end() const {
        return end_;
    }

private:
    decltype(std::begin(std::declval<Container&>())) begin_;
    decltype(std::end(std::declval<Container&>())) end_;

};

template<typename Container>
AsDataFlow(Container&) -> AsDataFlow<Container>;

template<typename Container>
AsDataFlow(Container&&) -> AsDataFlow<Container>;
