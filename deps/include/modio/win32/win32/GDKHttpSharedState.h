#pragma once
#include "fmt/format.h"
#include "modio/core/ModioErrorCode.h"
#include <atomic>
#include <memory>
#include <queue>
#include <unordered_map>
#include <utility>
#include "common/GDKHttpSharedState.h"

/// @brief Win32 http shared state is just a thin wrapper around the common functionality
class HttpSharedState : public std::enable_shared_from_this<HttpSharedState>, public HttpSharedStateBase
{
public:
	using HttpSharedStateBase::HttpSharedStateBase;
};
