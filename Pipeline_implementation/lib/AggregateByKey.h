#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "Pipeline.h"


template<typename Value, typename Aggregator, typename KeySelect> 
class AggregateByKey {
public:
    AggregateByKey(Value value, Aggregator aggregator, KeySelect key)
            : value_(std::move(value))
            , aggregator_(std::move(aggregator))
            , key_(std::move(key)) {}

    auto operator()(auto&& pipeline) {
        using KeyType = decltype(key_(*pipeline.begin()));
        std::vector<std::pair<KeyType, Value>> result_vec;

        for (const auto& element : pipeline) {
            auto current_key = key_(element);
            auto it = std::find_if(result_vec.begin(), result_vec.end(),
                [&current_key](const auto& pair) { return pair.first == current_key; });
            
            if (it == result_vec.end()) {
                result_vec.emplace_back(current_key, value_);
                it = result_vec.end() - 1;
            }
            aggregator_(element, it->second);   
        }
        
        return result_vec;
    }

private:
    Value value_;
    Aggregator aggregator_;
    KeySelect key_;

};
