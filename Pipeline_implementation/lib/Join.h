#pragma once

#include <cstddef>
#include <optional>
#include <utility>
#include <unordered_map>

#include "Pipeline.h"


template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;

    bool operator==(const KV& other) const {
        return key == other.key && value == other.value;
    }

    bool operator!=(const KV& other) const {
        return !(*this == other);
    }
};


template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

    bool operator==(const JoinResult& other) const {
        if (!(base == other.base)) {
            return false;
        }
        
        if (joined.has_value() != other.joined.has_value()) {
            return false;
        }
        
        if (joined && other.joined) {
            return *joined == *other.joined;
        }
        
        return true;
    }

    bool operator!=(const JoinResult& other) const {
        return !(*this == other);
    }
};


template<typename RightFlow, typename LeftKey = std::nullptr_t, typename RightKey = std::nullptr_t>
class JoinKey {
public:
    JoinKey(RightFlow rhs, LeftKey left_key, RightKey right_key)
            : rhs_(std::move(rhs))
            , right_key_(std::move(right_key))
            , left_key_(std::move(left_key)) {}

    
    auto operator()(auto&& lhs) {
        using LeftType = typename std::decay_t<decltype(*lhs.begin())>;
        using RightType = typename std::decay_t<decltype(*rhs_.begin())>;
        using ResultType = JoinResult<LeftType, RightType>;

        using KeyLeftType = decltype(left_key_(LeftType{}));
        using KeyRightType = decltype(right_key_(RightType{}));

        std::unordered_multimap<KeyRightType, RightType> right_map;
        
        for (auto&& right_data : rhs_) {
            auto&& right_key = right_key_(right_data);
            right_map.emplace(std::move(right_key), std::forward<decltype(right_data)>(right_data));
        }

        std::vector<ResultType> result_vec;

        for (auto&& left_item : lhs) {
            KeyLeftType left_key = left_key_(left_item);

            auto range = right_map.equal_range(left_key);
            if (range.first != range.second) {
                for (auto it = range.first; it != range.second; ++it) {
                    result_vec.emplace_back(ResultType{std::forward<decltype(left_item)>(left_item), it->second});
                }
            } else {
                result_vec.emplace_back(ResultType{std::forward<decltype(left_item)>(left_item), std::nullopt});
            }
        }

        return result_vec;
    }

private:
    RightFlow rhs_;
    RightKey right_key_;        
    LeftKey left_key_;

};


template<typename RightFlow, typename LeftKey = std::nullptr_t, typename RightKey = std::nullptr_t>
auto Join(RightFlow rhs, LeftKey left_key, RightKey right_key) {
    return JoinKey(std::move(rhs), std::move(left_key), std::move(right_key));
}


template<typename RightFlow>
class JoinKV {
public:
    JoinKV(RightFlow&& rhs) : rhs_(std::forward<RightFlow>(rhs)) {}

    auto operator()(auto&& lhs) {
        using RightType = typename std::decay_t<decltype(*std::begin(rhs_))>;
        using RightKeyType = decltype(RightType::key);
        using RightValueType = decltype(RightType::value);

        using LeftType = typename std::decay_t<decltype(*std::begin(lhs))>;
        using LeftValueType = decltype(LeftType::value);
        
        std::unordered_multimap<RightKeyType, RightValueType> right_map;
        for (auto&& right_data : rhs_) {
            right_map.emplace(right_data.key, std::forward<decltype(right_data.value)>(right_data.value));
        }

        using ResultType = JoinResult<LeftValueType, RightValueType>;

        std::vector<ResultType> result_vec;
        for (auto&& left_data : lhs) {
            auto right_data = right_map.equal_range(left_data.key);
            if (right_data.first == right_data.second) {
                result_vec.emplace_back(ResultType{
                    std::forward<decltype(left_data.value)>(left_data.value),
                    std::nullopt
                });
            } else {
                for (auto it = right_data.first; it != right_data.second; ++it) {
                    result_vec.emplace_back(ResultType{
                        std::forward<decltype(left_data.value)>(left_data.value),
                        it->second
                    });
                }
            }
        }

        return result_vec;
    }


private:
    RightFlow rhs_;

};


template<typename RightFlow>
auto Join(RightFlow&& rhs) {
    return JoinKV<RightFlow>(std::forward<RightFlow>(rhs));
}

