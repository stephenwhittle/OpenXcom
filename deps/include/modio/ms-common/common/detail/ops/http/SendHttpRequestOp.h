#pragma once
#include "common/GDKHttpSharedState.h"
#include "common/http/HttpRequestImplementation.h"
#include "modio/core/ModioErrorCode.h"
#include "modio/detail/AsioWrapper.h"
#include "modio/detail/FmtWrapper.h"
#include <memory>

#include <asio/yield.hpp>

class SendHttpRequestOp
{
	asio::coroutine CoroutineState;
	std::shared_ptr<HttpRequestImplementation> Request;
	std::weak_ptr<HttpSharedStateBase> SharedState;
	std::unique_ptr<asio::steady_timer> SendTimer;

public:
	SendHttpRequestOp(std::shared_ptr<HttpRequestImplementation> Request,
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
		auto ContextPtr = PinnedState.get();
		reenter(CoroutineState)
		{
			{
				Modio::ErrorCode InitStatus;
				PinnedState->InitializeRequest(Request, InitStatus);
				if (InitStatus)
				{
					Self.complete(InitStatus);
					return;
				}
			}
			for (Modio::Detail::HttpRequestParams::Header CurrentHeader : Request->Parameters.GetHeaders())
			{
				std::string FormattedHeader = fmt::format("{}: {}\r\n", CurrentHeader.first, CurrentHeader.second);
				if (!WinHttpAddRequestHeaders(Request->RequestHandle, (LPCWSTR) UTF8ToWideChar(FormattedHeader).c_str(),
											  -1L, WINHTTP_ADDREQ_FLAG_ADD))
				{
					auto err = GetLastError();
					Self.complete(Modio::make_error_code(Modio::HttpError::CannotOpenConnection));
					return;
				}
			}

			if (Request->Parameters.GetPayload())
			{
				const std::string& Payload = Request->Parameters.GetPayload().value();
				if (!WinHttpSendRequest(Request->RequestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
										(void*) Payload.c_str(), Payload.length(), Payload.length(),
										(DWORD_PTR) ContextPtr))
				{
					auto err = GetLastError();
					Self.complete(Modio::make_error_code(Modio::HttpError::CannotOpenConnection));
					return;
				}
			}
			else
			{
				if (!WinHttpSendRequest(Request->RequestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
										WINHTTP_NO_REQUEST_DATA, 0, 0, (DWORD_PTR) ContextPtr))
				{
					Self.complete(Modio::make_error_code(Modio::HttpError::CannotOpenConnection));
					return;
				}
			}

			if (SendTimer == nullptr)
			{
				SendTimer = std::make_unique<asio::steady_timer>(Modio::Detail::Services::GetGlobalContext());
			}

			while (PinnedState->PeekHandleStatus(Request->RequestHandle) == WinHTTPCallbackStatus::Waiting)
			{
				SendTimer->expires_after(std::chrono::milliseconds(1));
				yield SendTimer->async_wait(std::move(Self));
			}

			switch (PinnedState->FetchAndClearHandleStatus(Request->RequestHandle))
			{
				case WinHTTPCallbackStatus::RequestError:
					Self.complete(Modio::make_error_code(Modio::HttpError::RequestError));
					return;
				default:
					Self.complete(Modio::ErrorCode {});
					return;
			}
		}
	}
};
#include <asio/unyield.hpp>
