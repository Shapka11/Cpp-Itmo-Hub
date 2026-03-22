#pragma once

#include <concepts>
#include <iterator>

#include "AggregateByKey.h"
#include "AsDataFlow.h"
#include "AsVector.h"
#include "Dir.h"
#include "DropNullopt.h"
#include "Filter.h"
#include "Join.h"
#include "OpenFiles.h"
#include "Out.h"
#include "Pipeline.h"
#include "Split.h"
#include "SplitExpected.h"
#include "Transform.h"
#include "Write.h"


template <typename Pipeline, typename Adapter>
requires std::invocable<Adapter, Pipeline&&>
auto operator|(Pipeline&& pipeline, Adapter&& adapter) {
    return std::forward<Adapter>(adapter)(std::forward<Pipeline>(pipeline));
}
