#pragma once
#include "fmt/format.h"
#include "common/http/HttpRequestImplementation.h"
#include "modio/core/ModioLogger.h"
#include "common/UTF16Support.h"
#include <asio.hpp>
#include <memory>
#include "common/GDKHttpSharedState.h"
// clang-format off
#include <asio/yield.hpp>
// clang-format on
class ReadHttpResponseHeaders
{
	std::weak_ptr<HttpSharedStateBase> SharedState;
	std::shared_ptr<HttpRequestImplementation> Request;
	asio::coroutine CoroutineState;
	std::unique_ptr<asio::steady_timer> Timer;

public:
	ReadHttpResponseHeaders(std::shared_ptr<HttpRequestImplementation> Request,
							std::weak_ptr<HttpSharedStateBase> SharedState)
		: Request(Request),
		  SharedState(SharedState)
	{}

	template<typename CoroType>
	void operator()(CoroType& Self, Modio::ErrorCode ec = {})
	{
		std::shared_ptr<HttpSharedStateBase> PinnedState = SharedState.lock();
		if (PinnedState == nullptr || PinnedState->IsClosing())
		{
			Self.complete(Modio::make_error_code(Modio::GenericError::OperationCanceled));
			return;
		}

		reenter(CoroutineState)
		{
			if (!WinHttpReceiveResponse(Request->RequestHandle, 0))
			{
				Self.complete(Modio::make_error_code(Modio::HttpError::RequestError));
				return;
			}

			if (Timer == nullptr)
			{
				Timer = std::make_unique<asio::steady_timer>(Modio::Detail::Services::GetGlobalContext());
			}

			while (PinnedState->PeekHandleStatus(Request->RequestHandle) == WinHTTPCallbackStatus::Waiting)
			{
				Timer->expires_after(std::chrono::milliseconds(1));
				yield Timer->async_wait(std::move(Self));
			}

			switch (PinnedState->FetchAndClearHandleStatus(Request->RequestHandle))
			{
				case WinHTTPCallbackStatus::RequestError:
					Self.complete(Modio::make_error_code(Modio::HttpError::RequestError));
					return;
				default:

				{
					DWORD StatusCode = 0;
					DWORD BufferSize = sizeof(StatusCode);
					if (WinHttpQueryHeaders(
							Request->RequestHandle, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
							WINHTTP_HEADER_NAME_BY_INDEX, &StatusCode, &BufferSize, WINHTTP_NO_HEADER_INDEX))
					{
						Request->ResponseCode = StatusCode;
					}
					else
					{
						Modio::Detail::Logger().Log(Modio::LogLevel::Error, Modio::LogCategory::Http,
											"Unable to retrieve response headers for HTTP request");
						Self.complete(Modio::make_error_code(Modio::HttpError::RequestError));
						return;
					}
				}
					Self.complete(Modio::ErrorCode {});
					return;
			}
		}
	}
};
// clang-format off
#include <asio/unyield.hpp>
// clang-format on