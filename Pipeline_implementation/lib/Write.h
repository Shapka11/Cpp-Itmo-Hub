#pragma once

#include <utility>

#include "Pipeline.h"


template<typename OutFlow>
class Write {
public:
    Write(OutFlow& flow, const char& delimiter) : flow_(flow), delimiter_(delimiter) {};

    auto operator()(auto&& pipeline) {
        for (const auto& element : pipeline) {
            flow_ << element << delimiter_;
        }
        using IteratorType = decltype(pipeline.begin());
        return Pipeline<IteratorType>(
            std::move(pipeline.begin()),
            std::move(pipeline.end())
        );
    }

private:
    OutFlow& flow_;
    const char delimiter_;

};
