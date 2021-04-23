#pragma once

#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/ModioSDK.h"
#endif

#include "modio/core/ModioFilesystem.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/core/entities/ModioModDetails.h"
#include "modio/core/entities/ModioModInfo.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/core/entities/ModioModTagOptions.h"
#include "modio/detail/ops/mod/GetModDetails.h"
#include "modio/detail/ops/mod/GetModInfo.h"
#include "modio/detail/ops/mod/GetModMediaAvatar.h"
#include "modio/detail/ops/mod/GetModMediaGallery.h"
#include "modio/detail/ops/mod/GetModMediaLogo.h"
#include "modio/detail/ops/mod/GetModTags.h"
#include "modio/detail/ops/mod/ListAllMods.h"
#include "modio/impl/SDKPreconditionChecks.ipp"

// Implementation header - do not include directly

namespace Modio
{
	void ListAllModsAsync(FilterParams Filter,
						  std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfoList>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfoList>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::ModInfoList>)>(
				Modio::Detail::ListAllMods(Modio::Detail::SDKSessionData::CurrentGameID(),
										   std::move(Filter)),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void GetModInfoAsync(Modio::ModID ModId,
						 std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfo>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfo>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::ModInfo>)>(
				Modio::Detail::GetModInfo(Modio::Detail::SDKSessionData::CurrentGameID(),
										  Modio::Detail::SDKSessionData::CurrentAPIKey(), ModId),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}
// Disabled
#if (0)
	void GetModFileDetailsAsync(Modio::ModID ModId,
								std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModDetails>)> Callback)
	{
		return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModDetails>)>,
								   void(Modio::ErrorCode, Modio::Optional<Modio::ModDetails>)>(
			Modio::Detail::GetModDetails(Modio::Detail::SDKSessionData::CurrentGameID(),
										 Modio::Detail::SDKSessionData::CurrentAPIKey(), ModId),
			Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
	}
#endif
	void GetModMediaAsync(Modio::ModID ModId, Modio::LogoSize LogoSize,
						  std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>(
				Modio::Detail::GetModMediaLogo(Modio::Detail::SDKSessionData::CurrentGameID(),
											   Modio::Detail::SDKSessionData::CurrentAPIKey(), ModId, LogoSize),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void GetModMediaAsync(Modio::ModID ModId, Modio::AvatarSize AvatarSize,
						  std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>(
				Modio::Detail::GetModMediaAvatar(Modio::Detail::SDKSessionData::CurrentGameID(),
												 Modio::Detail::SDKSessionData::CurrentAPIKey(), ModId, AvatarSize),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void GetModMediaAsync(
		Modio::ModID ModId, Modio::GallerySize GallerySize, Modio::GalleryIndex Index,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)>(
				Modio::Detail::GetModMediaGallery(Modio::Detail::SDKSessionData::CurrentGameID(),
												  Modio::Detail::SDKSessionData::CurrentAPIKey(), ModId, GallerySize,
												  Index),
				Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

	void GetModTagOptionsAsync(std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModTagOptions>)> Callback)
	{
		if (Modio::Detail::RequireSDKIsInitialized(Callback) && Modio::Detail::RequireNotRateLimited(Callback))
		{
			return asio::async_compose<std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModTagOptions>)>,
									   void(Modio::ErrorCode, Modio::Optional<Modio::ModTagOptions>)>(
				Modio::Detail::GetModTags(), Callback, Modio::Detail::Services::GetGlobalContext().get_executor());
		}
	}

} // namespace Modio
