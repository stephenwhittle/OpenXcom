#pragma once

#include "modio/detail/ModioDefines.h"
#include "modio/core/ModioFilterParams.h"
#include "modio/core/ModioModCollectionEntry.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/core/entities/ModioModDetails.h"
#include "modio/core/entities/ModioModTagOptions.h"
#include "modio/core/entities/ModioUser.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/detail/ModioLibraryConfigurationHelpers.h"

namespace Modio
{
	/// @docpublic
	/// @brief Initializes the SDK for the given user. Loads the state of mods installed on the system as well as the
	/// set of mods the specified user has installed on this device
	/// @param GameID The Mod.io-provided ID for the game
	/// @param APIKey the Mod.io-provided API key for your application or game
	/// @param User The user handle for the platform you are targeting:
	/// @param OnInitComplete Callback which will be invoked with the result of initialization
	MODIO_API void InitializeAsync(Modio::GameID GameID, Modio::ApiKey APIKey, Modio::Environment GameEnvironment,
								   Modio::UserHandleType User, std::function<void(Modio::ErrorCode)> OnInitComplete);

	/// @docpublic
	/// @brief Sets the global logging level - messages with a log level below the specified value will not be displayed
	/// @param Level Value indicating which priority of messages should be included in the log output
	/// @return
	MODIO_API void SetLogLevel(Modio::LogLevel Level);

	/// @docpublic
	/// @brief Provide a callback to handle log messages emitted by the SDK.
	/// @param LogCallback Callback invoked by the SDK during xref:Modio::RunPendingHandlers[] for each log emitted
	/// during that invocation
	/// @return
	MODIO_API void SetLogCallback(std::function<void(Modio::LogLevel, const std::string&)> LogCallback);

	/// @docpublic
	/// @brief Runs any pending SDK work on the calling thread, including invoking any callbacks passed to asynchronous
	/// operations.
	/// NOTE: This should be called while xref:Modio::InitializeAsync[] and xref:Modio::Shutdown[] are running,
	/// as they both utilise the internal event loop for functionality.
	/// @return
	MODIO_API void RunPendingHandlers();

	/// @docpublic
	/// @brief Cancels any running internal operations, frees SDK resources, and invokes any pending callbacks with
	/// Modio::GenericError::OperationCanceled . This function will block while the deinitialization occurs.
	/// @return
	MODIO_API void Shutdown();

	/// @docpublic
	/// @brief Cancels any running internal operations and invokes any pending callbacks with
	/// Modio::GenericError::OperationCanceled. This function does not block; you should keep calling
	/// Modio::RunPendingHandlers until the callback you provide to this function is invoked.
	/// @param OnShutdownComplete
	/// @requires initialized-sdk
	MODIO_API void ShutdownAsync(std::function<void(Modio::ErrorCode)> OnShutdownComplete);

	/// @docpublic
	/// @brief Sends a request to the Mod.io server to add the specified mod to the user's list of subscriptions, and
	/// marks the mod for local installation by the SDK
	/// @param ModToSubscribeTo Mod ID of the mod requiring a subscription.
	/// @param OnSubscribeComplete Callback invoked when the subscription request is completed.
	/// @requires initialized-sdk
	/// @requires authenticated-user
	/// @requires no-rate-limiting
	MODIO_API void SubscribeToModAsync(Modio::ModID ModToSubscribeTo,
									   std::function<void(Modio::ErrorCode)> OnSubscribeComplete);

	/// @docpublic
	/// @brief Sends a request to the Mod.io server to remove the specified mod from the user's list of subscriptions.
	/// If no other local users are subscribed to the specified mod this function will also mark the mod for
	/// uninstallation by the SDK.
	/// @param ModToUnsubscribeFrom Mod ID of the mod requiring unsubscription.
	/// @param OnUnsubscribeComplete Callback invoked when the unsubscription request is completed.
	/// @requires initialized-sdk
	/// @requires authenticated-user
	/// @requires no-rate-limiting
	MODIO_API void UnsubscribeFromModAsync(Modio::ModID ModToUnsubscribeFrom,
										   std::function<void(Modio::ErrorCode)> OnUnsubscribeComplete);

	/// @docpublic
	/// @brief Synchronises the local list of the current user's subscribed mods with the server. Any mods that have
	/// been externally subscribed will be automatically marked for installation, and mods that have been externally
	/// removed from the user's subscriptions may be uninstalled if no other local users have a current subscription.
	/// Calling this before you call <<QueryUserSubscriptions>>, <<QueryUserInstallations>> or
	/// <<QuerySystemInstallations>> will ensure that if the system mod directory has been moved or relocated, those
	/// functions will still return correct values.
	/// @param OnFetchDone Callback invoked when the external state has been retrieved and merged with the local data
	/// @requires initialized-sdk
	/// @requires authenticated-user
	/// @requires no-rate-limiting
	MODIO_API void FetchExternalUpdatesAsync(std::function<void(Modio::ErrorCode)> OnFetchDone);

	/// @docpublic
	/// @brief Enables the automatic management of installed mods on the system based on the user's subscriptions.
	/// @param Callback This callback handler will be invoked with a ModManagementEvent for each mod operation performed
	/// by the SDK
	MODIO_API void EnableModManagement(std::function<void(Modio::ModManagementEvent)> Callback);

	/// @docpublic
	/// @brief Disables automatic installation or uninstallation of mods based on the user's subscriptions. Allows
	/// currently processing installation to complete; will cancel any pending operations when called.
	MODIO_API void DisableModManagement();

	/// @docpublic
	/// @brief Checks if the automatic management process is currently installing or removing mods
	/// @return True if automatic management is currently performing an operation
	MODIO_API bool IsModManagementBusy();

	/// @docpublic
	/// @brief Provides progress information for a mod installation or update operation if one is currently in progress.
	/// @return Optional ModProgressInfo object containing information regarding the progress of the installation
	/// operation.
	MODIO_API Modio::Optional<Modio::ModProgressInfo> QueryCurrentModUpdate();

	/// @docpublic
	/// @brief Fetches the local view of the user's subscribed mods, including mods that are subscribed but not yet
	/// installed
	/// @return std::map using Mod IDs as keys and ModCollectionEntry objects providing information about the subscribed
	/// mods
	MODIO_API std::map<Modio::ModID, Modio::ModCollectionEntry> QueryUserSubscriptions();

	/// @docpublic
	/// @brief Fetches the subset of the user's subscribed mods that are installed and therefore ready for loading
	/// @param bIncludeOutdatedMods Include subscribed mods that are installed but have an updated version on the server
	/// that has not yet been installed
	/// @return std::map using Mod IDs as keys and ModCollectionEntry objects providing information about the subscribed
	/// mods
	MODIO_API std::map<Modio::ModID, Modio::ModCollectionEntry> QueryUserInstallations(bool bIncludeOutdatedMods);

	/// @docpublic
	/// @brief Fetches all mods installed on the system such that a consuming application can present the information in
	/// a UI in order to free up space by uninstalling mods
	/// @return std::map using Mod IDs as keys and ModCollectionEntry objects providing information about mods installed
	/// on the system regardless of which user installed them
	MODIO_API std::map<Modio::ModID, Modio::ModCollectionEntry> QuerySystemInstallations();

	/// @docpublic
	/// @brief Forcibly uninstalls a mod from the system. This is intended for use when a host application requires more
	/// room for a mod that the user wants to install, and as such will return an error if the current user is
	/// subscribed to the mod. To remove a mod the current user is subscribed to, use
	/// xref:Modio::UnsubscribeFromModAsync[].
	/// @param Callback Callback invoked when the uninstallation is successful, or if it failed because the current user
	/// remains subscribed.
	MODIO_API void ForceUninstallModAsync(Modio::ModID ModToRemove, std::function<void(Modio::ErrorCode)> Callback);

	/// @docpublic
	/// @brief Fetches the currently authenticated Mod.io user profile if there is one associated with the current
	/// platform user
	/// @return Optional Modio::User object containing profile information
	MODIO_API Modio::Optional<Modio::User> QueryUserProfile();

	/// @docpublic
	/// @brief Uses platform-specific authentication to associate a Mod.io user account with the current platform user
	/// @param User Authentication payload data to submit to the provider
	/// @param Provider The provider to use to perform the authentication
	/// @param Callback Callback invoked once the authentication request has been made
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void AuthenticateUserExternalAsync(Modio::AuthenticationParams User,
												 Modio::AuthenticationProvider Provider,
												 std::function<void(Modio::ErrorCode)> Callback);

	/// @docpublic
	/// @brief Provides a list of mods for the current game, that match the parameters specified in the filter
	/// @param Filter Modio::FilterParams object containing any filters that should be applied to the query
	/// @param Callback Callback invoked with a status code and an optional ModInfoList providing mod profiles
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void ListAllModsAsync(
		Modio::FilterParams Filter,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfoList>)> Callback);

	/// @docpublic
	/// @brief Fetches detailed information about the specified mod, including description and file metadata for the
	/// most recent release
	/// @param ModId Mod ID of the mod to fetch data
	/// @param Callback Callback providing a status code and an optional Modio::ModInfo object with the mod's extended
	/// information
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void GetModInfoAsync(Modio::ModID ModId,
								   std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModInfo>)> Callback);

	/// @docpublic
	/// @brief Downloads the logo for the specified mod. Will use existing file if it is already present on disk
	/// @param ModId Mod ID for use in logo retrieval
	/// @param LogoSize Parameter indicating the size of logo that's required
	/// @param Callback Callback providing a status code and an optional path object pointing to the location of the
	/// downloaded image
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void GetModMediaAsync(
		Modio::ModID ModId, Modio::LogoSize LogoSize,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback);

	/// @docpublic
	/// @brief Get a gallery image for the specified mod ID. If it already exists on disk the file will be reused unless
	/// it is outdated
	/// @param ModId The mod you want to retrieve an image for
	/// @param GallerySize Size of the image you want to retrieve
	/// @param Index The zero-based index of the image you want to retrieve
	/// @param Callback Callback containing a status code and an Optional containing a path to the image file on disk
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void GetModMediaAsync(
		Modio::ModID ModId, Modio::GallerySize GallerySize, Modio::GalleryIndex Index,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback);

	/// @docpublic
	/// @brief Downloads the creator avatar for a specified mod. Will use existing file if it is already present on disk
	/// and not outdated
	/// @param ModId ID of the mod the creator avatar will be retrieved for
	/// @param AvatarSize Parameter indicating the size of avatar image that's required
	/// @param Callback Callback providing a status code and an optional path object pointing to the location of the
	/// downloaded image
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void GetModMediaAsync(
		Modio::ModID ModId, Modio::AvatarSize AvatarSize,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback);

	/// @docpublic
	/// @brief Clears the internal HTTP cache
	/// @requires initialized-sdk
	MODIO_API void ClearCache();

	/// @docpublic
	/// @brief Fetches the available tags used on mods for the current game. These tags can them be used in conjunction
	/// with the FilterParams passed to ListAllMods
	/// @param Callback Callback providing a status code and an optional ModTagOptions object containing the available
	/// tags
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	MODIO_API void GetModTagOptionsAsync(
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::ModTagOptions>)> Callback);

	/// @docpublic
	/// @brief Begins email authentication for the current session by requesting a one-time code be sent to the
	/// specified email address if it is associated with a Mod.io account
	/// @param EmailAddress The email address to send the code to
	/// @param Callback Callback providing a status code indicating the outcome of the request
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	/// @requires no-authenticated-user
	MODIO_API void RequestEmailAuthCodeAsync(Modio::EmailAddress EmailAddress,
											 std::function<void(Modio::ErrorCode)> Callback);

	/// @docpublic
	/// @brief Completes email authentication for the current session by submitting the one-time code sent to the user's
	/// email address
	/// @param AuthenticationCode User's authentication code
	/// @param Callback Callback providing a status code indicating if authentication was successful or not
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	/// @requires no-authenticated-user
	MODIO_API void AuthenticateUserEmailAsync(Modio::EmailAuthCode AuthenticationCode,
											  std::function<void(Modio::ErrorCode)> Callback);

	/// @docpublic
	/// @brief De-authenticates the current Mod.io user for the current session, and clears all user-specific data
	/// stored on the current device. Any subscribed mods that are installed but do not have other local users
	/// subscribed will be uninstalled
	/// @param Callback Callback providing a status code indicating the outcome of clearing the user data. Error codes
	/// returned by this function are informative only - it will always succeed.
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	/// @requires authenticated-user
	MODIO_API void ClearUserDataAsync(std::function<void(Modio::ErrorCode)> Callback);

	/// @docpublic
	/// @brief Downloads the avatar of the currently authenticated user. Will only perform a download if there is no
	/// local cache of the avatar or if that cached copy is out-of-date.
	/// @param AvatarSize Parameter specifying the size of avatar image to download
	/// @param Callback Callback providing a status code for the download and an optional path to the downloaded image
	/// @requires initialized-sdk
	/// @requires no-rate-limiting
	/// @requires authenticated-user
	MODIO_API void GetUserMediaAsync(
		Modio::AvatarSize AvatarSize,
		std::function<void(Modio::ErrorCode, Modio::Optional<Modio::filesystem::path>)> Callback);

} // namespace Modio

// Implementation headers

#include "modio/detail/ModioUndefs.h"
#ifndef MODIO_SEPARATE_COMPILATION
	#include "modio/impl/SDKCore.ipp"
	#include "modio/impl/SDKModManagement.ipp"
	#include "modio/impl/SDKModMetadata.ipp"
	#include "modio/impl/SDKUserData.ipp"
#endif