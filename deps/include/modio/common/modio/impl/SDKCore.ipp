#pragma once

#ifdef MODIO_SEPARATE_COMPILATION
#include "modio/ModioSDK.h"
#endif

#include "modio/cache/ModioCacheService.h"
#include "modio/detail/ModioSDKSessionData.h"
#include "modio/detail/ops/AsyncShutdown.h"
#include "modio/detail/ops/ServiceInitialization.h"
#include "modio/file/ModioFileService.h"
#include "modio/http/ModioHttpService.h"
#include "modio/impl/SDKPostAsync.ipp"
#include "modio/impl/SDKPreconditionChecks.ipp"
#include "modio/userdata/ModioUserDataService.h"
// Implementation header - do not include directly

#include <functional>

namespace Modio
{
	void InitializeAsync(Modio::InitializeOptions InitOptions, std::function<void(Modio::ErrorCode)> OnInitComplete)
	{
		if (Modio::Detail::RequireValidInitParams(InitOptions, OnInitComplete))
		{
			auto WrappedCallback = Modio::Detail::ApplyPostAsyncChecks(OnInitComplete);
			return asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
				ServiceInitialization(InitOptions.GameID, std::move(InitOptions.APIKey), InitOptions.GameEnvironment,
									  InitOptions.User),
				WrappedCallback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void ClearCache()
	{
		Modio::Detail::Services::GetGlobalService<Modio::Detail::CacheService>().ClearCache();
	}

	void RunPendingHandlers()
	{
		// Run any pending handlers on the global io_context
		if (Modio::Detail::Services::GetGlobalContext().stopped())
		{
			Modio::Detail::Services::GetGlobalContext().restart();
		}
		Modio::Detail::Services::GetGlobalContext().poll();

		// invoke the mod management log callback if the user has set it
		Modio::Detail::SDKSessionData::FlushModManagementLog();
		// invoke log callback if the user has set it
		Modio::Detail::Services::GetGlobalService<Modio::Detail::LogService>().FlushLogBuffer();
	}

#ifndef MODIO_SEPARATE_COMPILATION
	// Forward declaration
	void DisableModManagement();
#endif
	

	void Shutdown()
	{
		// We need to be initialized to shutdown
		if (!Modio::Detail::SDKSessionData::IsInitialized())
		{
			return;
		}

		Modio::DisableModManagement();
		Modio::Detail::Services::GetGlobalService<Modio::Detail::CacheService>().Shutdown();
		Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>().Shutdown();
		Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().Shutdown();
		Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().Shutdown();
		Modio::Detail::Services::GetGlobalService<Modio::Detail::LogService>().Shutdown();
		Modio::Detail::SDKSessionData::Deinitialize();
		auto OldContext = Modio::Detail::Services::ResetGlobalContext();
		auto ExhaustOldContext = [OldContext]() mutable {
			OldContext->restart();
			OldContext->run();
			OldContext.reset();
		};
		asio::post(Modio::Detail::Services::GetGlobalContext().get_executor(), std::move(ExhaustOldContext));

		// Ensure that we run the exhaust function and empties out the work queue
		Modio::Detail::Services::GetGlobalContext().restart();
		Modio::Detail::Services::GetGlobalContext().run();
	}

	// This might need a timeout parameter
	void ShutdownAsync(std::function<void(Modio::ErrorCode)> OnShutdownComplete)
	{
		if (Modio::Detail::RequireSDKIsInitialized(OnShutdownComplete))
		{
			// Halt the mod management loop
			Modio::DisableModManagement();
			// Signal the internal services their operations should shut down ASAP
			Modio::Detail::Services::GetGlobalService<Modio::Detail::CacheService>().Shutdown();
			Modio::Detail::Services::GetGlobalService<Modio::Detail::UserDataService>().Shutdown();
			Modio::Detail::Services::GetGlobalService<Modio::Detail::HttpService>().Shutdown();
			Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().Shutdown();
			Modio::Detail::Services::GetGlobalService<Modio::Detail::LogService>().Shutdown();
			Modio::Detail::SDKSessionData::Deinitialize();
			// Steal the old io context (and associated services)
			auto OldContext = Modio::Detail::Services::ResetGlobalContext();
			// Post to the new io_context an operation that will exhaust the old context and call the user callback when
			// it's completed
			asio::async_compose<std::function<void(Modio::ErrorCode)>, void(Modio::ErrorCode)>(
				Modio::Detail::AsyncShutdown(std::move(OldContext)), OnShutdownComplete,
				Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void SetLogLevel(Modio::LogLevel Level)
	{
		Modio::Detail::LogService::SetGlobalLogLevel(Level);
	}

	void SetLogCallback(std::function<void(Modio::LogLevel Level, const std::string& Message)> LogCallback)
	{
		Modio::Detail::LogService::SetLogCallback(LogCallback);
	}
} // namespace Modio
