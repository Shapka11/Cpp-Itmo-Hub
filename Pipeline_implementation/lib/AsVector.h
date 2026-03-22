#pragma once

#include <vector>
#include <utility>

#include "Pipeline.h"


class AsVector {
public:
    AsVector() = default;

    auto operator()(auto&& pipeline) {
        using Type = std::decay_t<decltype(*pipeline.begin())>;
        std::vector<Type> result_vec;

        for (const auto& i : pipeline) {
            result_vec.emplace_back(std::forward<decltype(i)>(i));
        }

        return result_vec;
    }

};
    