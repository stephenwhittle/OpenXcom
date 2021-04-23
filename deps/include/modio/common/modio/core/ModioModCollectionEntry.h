#pragma once
#include "modio/core/ModioLogger.h"
#include "modio/core/ModioStdTypes.h"
#include "modio/core/entities/ModioModInfo.h"
#include "modio/detail/ModioConstants.h"
#include "modio/detail/ModioTransactional.h"
#include "modio/file/ModioFileService.h"
#include <algorithm>
#include <atomic>
#include <memory>
#include <set>

namespace Modio
{
	/// @docpublic
	/// @brief Enum representing the current state of a mod
	enum class ModState
	{
		InstallationPending, // dont save
		Installed,
		UpdatePending, // saved as installed
		Downloading, // installing - dont save
		Extracting, // installing- don't save
		UninstallPending, // saved as installed
	};

	/// @docpublic
	/// @brief Class representing a mod which is installed locally
	class ModCollectionEntry : public Modio::Detail::Transactional<ModCollectionEntry>
	{
		/// <summary>
		/// The ID of the mod
		/// </summary>
		Modio::ModID ID;
		/// <summary>
		/// The status of the mod, is it pending installation, is it installed but needing an update, etc
		/// </summary>
		std::atomic<Modio::ModState> CurrentState = Modio::ModState::InstallationPending;

		Modio::Optional<Modio::ModState> RollbackState;

		/// <summary>
		/// REST API object containing information about the mod
		/// </summary>
		ModInfo ModProfile;
		/// <summary>
		/// Reference count for automatic deinstallation of mods not in use by any local users
		/// </summary>
		std::atomic<uint8_t> LocalUserSubscriptionCount = 0;
		std::set<Modio::UserID> LocalUserSubscriptions;
		Modio::filesystem::path PathOnDisk;
		Modio::FileSize SizeOnDisk;

		/// @docinternal
		/// @brief flag if this mod should not be processed because it encountered an unrecoverable error during
		/// installation/update should not be saved to disk as we will more than likely want to retry next session
		std::atomic<bool> ShouldNotRetry = false;

	public:
		ModCollectionEntry() = default;

		/// @docinternal
		/// @brief Constructor creating a ModCollection entry for the specified mod profile. Sets the default state to
		/// InstallationPending
		/// @param ProfileData Mod profile to create a collection entry for
		ModCollectionEntry(ModInfo ProfileData)
			: ID(ProfileData.ModId),
			  CurrentState(Modio::ModState::InstallationPending),
			  ModProfile(ProfileData),
			  LocalUserSubscriptions(),
			  PathOnDisk(Modio::Detail::Services::GetGlobalService<Modio::Detail::FileService>().MakeModPath(ID)) {};

		ModCollectionEntry(const ModCollectionEntry& Other)
			: ID(Other.ID),
			  CurrentState(Other.CurrentState.load()),
			  ModProfile(Other.ModProfile),
			  LocalUserSubscriptionCount(Other.LocalUserSubscriptionCount.load()),
			  LocalUserSubscriptions(Other.LocalUserSubscriptions),
			  PathOnDisk(Other.PathOnDisk)
		{}

		ModCollectionEntry& operator=(const ModCollectionEntry& Other)
		{
			ID = Other.ID;
			CurrentState.store(Other.CurrentState.load());
			ModProfile = Other.ModProfile;
			LocalUserSubscriptions = Other.LocalUserSubscriptions;
			LocalUserSubscriptionCount.store(Other.LocalUserSubscriptionCount.load());
			PathOnDisk = Other.PathOnDisk;
			return *this;
		}

		/// @docinternal
		/// @brief Updates the associated profile data in the collection entry, marks the mod as requiring an update if
		/// necessary
		/// @param ProfileData New profile data
		void UpdateModProfile(ModInfo ProfileData)
		{
			// check version in metadata and set pending install if need be
			if (ModProfile.FileInfo.MetadataId != ProfileData.FileInfo.MetadataId)
			{
				SetModState(ModState::UpdatePending);
			}
			ModProfile = ProfileData;
		}

		/// @docinternal
		/// @brief Increases the local user subscription count for the associated mod
		/// @return the new number of local user subscriptions
		uint8_t AddLocalUserSubscription(Modio::Optional<User> User)
		{
			if (User.has_value())
			{
				if (CurrentState == Modio::ModState::UninstallPending)
				{
					CurrentState.store(ModState::Installed);
				}

				LocalUserSubscriptions.insert(User->UserId);
			}
			return LocalUserSubscriptions.size();
		}

		/// @docinternal
		/// @brief Decrements the local user subscription count for the associated mod. Use with care - this
		/// subscription count is used to determine if a mod requires uninstallation from local storage
		/// @return The updated user subscription count
		uint8_t RemoveLocalUserSubscription(Modio::Optional<Modio::User> User)
		{
			if (User.has_value())
			{
				LocalUserSubscriptions.erase(User->UserId);
				if (LocalUserSubscriptions.size() == 0)
				{
					CurrentState.store(ModState::UninstallPending);
					Modio::Detail::Logger().Log(Modio::LogLevel::Info, Modio::LogCategory::ModManagement,
												"Reference count for mod {} now 0, marking for uninstallation", ID);
				}
			}
			return LocalUserSubscriptions.size();
		}

		/// @docinternal
		/// @brief Updates the state of the associated mod - this is used by the mod management subsystem to determine
		/// if a mod requires install/uninstall/update
		/// @param NewState The new state the mod should have
		void SetModState(Modio::ModState NewState)
		{
			CurrentState = NewState;
		}

		/// @docinternal
		/// @brief Marks the mod as having encountered an error that means installation or updates should not be
		/// reattempted this session
		void MarkModNoRetry()
		{
			ShouldNotRetry.store(true);
		}

		/// @docinternal
		/// @brief Clears the no retry flag - this probably won't ever get called but may if we want to retry everything
		/// after free space has been made or something similar
		/// We could probably do something cleaner with logic about which types of errors occurred and more selectively
		/// clear flags, but that can wait until the error code overhaul.
		void ClearModNoRetry()
		{
			ShouldNotRetry.store(false);
		}

		/// @docinternal
		/// @brief Fetches the retry flag for this mod
		/// @return if the mod's last error was one that permits retries
		bool ShouldRetryInstallation()
		{
			return !ShouldNotRetry.load();
		}

		/// @docpublic
		/// @return Modio::ModState enum representing current state of the mod
		Modio::ModState GetModState() const
		{
			return CurrentState;
		}

		/// @docpublic
		/// @return Mod ID
		Modio::ModID GetID() const
		{
			return ID;
		}

		/// @docpublic
		/// @return Modio::ModInfo containing mod profile data
		Modio::ModInfo GetModProfile() const
		{
			return ModProfile;
		}

		/// @docpublic
		/// @return Path to the mod's installation folder on disk
		/// NOTE: If the mod is not yet installed this path may not yet exist. Check
		/// @doc_xref{ModCollectionEntry::GetModState} before trying to load files in this location
		Modio::filesystem::path GetPath() const
		{
			return PathOnDisk;
		}

		/// @docpublic
		/// @brief Updates the size of the mod on disk in the collection entry. Called by the archive extraction code on
		/// successful extraction/installation
		/// @param NewSize The total size on disk of all files in the mod
		void UpdateSizeOnDisk(Modio::FileSize NewSize)
		{
			SizeOnDisk = NewSize;
		}

		friend void to_json(nlohmann::json& j, const ModCollectionEntry& Entry)
		{
			j = nlohmann::json::object(
				{{Modio::Detail::Constants::JSONKeys::ModEntryID, Entry.ID},
				 {Modio::Detail::Constants::JSONKeys::ModEntryProfile, Entry.ModProfile},
				 {Modio::Detail::Constants::JSONKeys::ModEntrySubCount, Entry.LocalUserSubscriptions},
				 {Modio::Detail::Constants::JSONKeys::ModEntryState, Entry.CurrentState.load()},
				 {Modio::Detail::Constants::JSONKeys::ModSizeOnDisk, Entry.SizeOnDisk}});
		}

		friend void from_json(const nlohmann::json& j, ModCollectionEntry& Entry)
		{
			Modio::Detail::ParseSafe(j, Entry.ID, Modio::Detail::Constants::JSONKeys::ModEntryID);
			Modio::Detail::ParseSafe(j, Entry.ModProfile, Modio::Detail::Constants::JSONKeys::ModEntryProfile);
			Modio::Detail::ParseSafe(j, Entry.LocalUserSubscriptions,
									 Modio::Detail::Constants::JSONKeys::ModEntrySubCount);
			Modio::Detail::ParseSafe(j, Entry.SizeOnDisk, Modio::Detail::Constants::JSONKeys::ModSizeOnDisk);
			Modio::ModState StateTmp = ModState::InstallationPending;
			Modio::Detail::ParseSafe(j, StateTmp, Modio::Detail::Constants::JSONKeys::ModEntryState);
			Entry.CurrentState.store(StateTmp);
		}

		friend void BeginTransactionImpl(ModCollectionEntry& Entry)
		{
			if (Entry.CurrentState == Modio::ModState::Downloading ||
				Entry.CurrentState == Modio::ModState::Extracting || Entry.RollbackState.has_value())
			{
				Modio::Detail::Logger().Log(
					Modio::LogLevel::Warning, Modio::LogCategory::ModManagement,
					"Beginning transaction on a mod in the system collection while a transaction is "
					"already occurring!");
				return;
			}
			else
			{
				Entry.RollbackState = Entry.CurrentState.load();
			}
		}

		friend void RollbackTransactionImpl(ModCollectionEntry& Entry)
		{
			if (!Entry.RollbackState.has_value())
			{
				Modio::Detail::Logger().Log(
					Modio::LogLevel::Warning, Modio::LogCategory::ModManagement,
					"Tried to rollback a transaction on a mod in the system collection while no "
					"transaction was occurring!");
				return;
			}
			else
			{
				Entry.CurrentState.store(Entry.RollbackState.take().value());
			}
		}

	private:
	};

	// TODO: @modio-core refactor ModProgressInfo to expose Total, Current, and State (hiding internal members)

	/// @docpublic
	/// @brief Class representing the progress of a mod installation or update
	class ModProgressInfo
	{
	public:
		// requires enum indicating if we're downloading or installing?

		/// @brief Total size of the downloaded file
		Modio::FileSize TotalDownloadSize;

		/// @brief Current amount downloaded in bytes
		Modio::FileSize CurrentlyDownloadedBytes;

		/// @brief Total size on disk when fully extracted
		Modio::FileSize TotalExtractedSizeOnDisk;

		/// @brief Amount of data currently extracted
		Modio::FileSize CurrentlyExtractedBytes;

		/// @brief The mod ID of the mod being processed
		Modio::ModID ID;

		ModProgressInfo(Modio::ModID ID) : ID(ID) {};
	};

	/// @doc_internal
	/// @brief Class containing the mod IDs the current user is subscribed to
	class UserSubscriptionList
	{
	public:
		/// <summary>
		/// Constructs an empty User subscription list
		/// </summary>
		UserSubscriptionList() {};
		/// <summary>
		/// Constructs a user subscription list by copying the Mod IDs from the provided vector
		/// </summary>
		/// <param name="NewIDs">Vector of Mod IDs to copy</param>
		UserSubscriptionList(std::vector<Modio::ModID> NewIDs) : InternalList(NewIDs.begin(), NewIDs.end()) {};
		/// <summary>
		/// Constructs a user subscription list by moving elements from the provided vector
		/// </summary>
		/// <param name="NewIDs">Vector of Mod IDs to move from</param>
		UserSubscriptionList(std::vector<Modio::ModID>&& NewIDs)
			: InternalList(std::make_move_iterator(NewIDs.begin()), std::make_move_iterator(NewIDs.end())) {};

		/// <summary>
		/// Adds a new mod to the subscription list
		/// </summary>
		/// <param name="Mod">The mod ID to add</param>
		/// <returns>True if the ID was inserted, false if the mod was already in the list</returns>
		bool AddMod(Modio::ModInfo Mod)
		{
			return InternalList.insert(Mod.ModId).second;
		}
		/// <summary>
		/// Removes a mod from the subscription list
		/// </summary>
		/// <param name="Mod">The mod ID to remove</param>
		void RemoveMod(Modio::ModID Mod)
		{
			InternalList.erase(Mod);
		}

		const std::set<Modio::ModID>& Get() const
		{
			return InternalList;
		}

		/// <summary>
		/// Enum indicating if a mod was added or removed when calculating the difference of two user subscription lists
		/// </summary>
		enum class ChangeType
		{
			Added,
			Removed
		};

		// will need an and operation on the user subscription list

		/// <summary>
		/// Calculates removals or additions between the two user subscription lists
		/// </summary>
		/// <param name="Original">The original list of subscriptions</param>
		/// <param name="Updated">the updated list of subscriptions</param>
		/// <returns>Map containing all CHANGED mod IDs, with ChangeType value indicating if the change was an addition
		/// or a removal</returns>
		static std::map<Modio::ModID, ChangeType> CalculateChanges(const UserSubscriptionList& Original,
																   const UserSubscriptionList& Updated)
		{
			std::map<Modio ::ModID, ChangeType> Diff;

			std::vector<Modio::ModID> ChangedMods;
			// Get a vector of all ModIDs that only occur in either Original or Updated but not both
			std::set_symmetric_difference(Original.InternalList.begin(), Original.InternalList.end(),
										  Updated.InternalList.begin(), Updated.InternalList.end(),
										  std::back_inserter(ChangedMods));
			// For each Mod ID that was present only in one set...
			for (Modio::ModID ChangedModID : ChangedMods)
			{
				// If the Mod ID not present in the original set, it must be an addition
				if (Original.InternalList.find(ChangedModID) == Original.InternalList.end())
				{
					Diff[ChangedModID] = ChangeType::Added;
				}
				else
				{
					// if the mod ID was present in the original set, it must NOT be in the new set. Therefore it is a
					// removal.
					Diff[ChangedModID] = ChangeType::Removed;
				}
			}

			return Diff;
		}
		friend void to_json(nlohmann::json& j, const UserSubscriptionList& List)
		{
			j = nlohmann::json {Modio::Detail::Constants::JSONKeys::UserSubscriptionList, List.InternalList};
		}
		friend void from_json(const nlohmann::json& j, UserSubscriptionList& List)
		{
			j.get_to(List.InternalList);
		}

	private:
		std::set<Modio::ModID> InternalList;
	};

	// vector of these for the log

	/// @docpublic
	/// @brief Simple struct representing the outcome of a mod management operation
	struct ModManagementEvent
	{
		/// @brief What type of event occurred
		enum class EventType
		{
			Installed, /** Mod installation to local storage */
			Uninstalled, /** Mod uninstallation from local storage*/
			Updated /** Mod local installation updated to latest version*/
		};

		/// @brief ID for the mod that the event occurred on
		Modio::ModID ID;

		/// @brief What type of event occurred
		EventType Event;

		/// @brief Empty if operation completed successfully, truthy/contains error code if operation failed
		Modio::ErrorCode Status;
	};

	class ModCollection
	{
	public:
		const ModCollection FilterByUserSubscriptions(const UserSubscriptionList& UserSubscriptions) const
		{
			ModCollection FilteredCollection;
			for (Modio::ModID UserModID : UserSubscriptions.Get())
			{
				if (ModEntries.count(UserModID))
				{
					FilteredCollection.ModEntries[UserModID] = ModEntries.at(UserModID);
				}
				else
				{
					// Silently fail for now, don't spam the log - empty list should only occur before first
					// FetchExternalUpdatesAsync
					/*Modio::Detail::Logger().Log(Modio::LogLevel::Warning, Modio::LogCategory::ModManagement,
										"Trying to look up mod ID {} but it is not present in local metadata",
										UserModID);*/
				}
			}
			return FilteredCollection;
		}

		/// <summary>
		/// Either inserts a mod into the collection or updates its metadata
		/// </summary>
		/// <param name="ModToAdd"></param>
		/// <returns>True if mod was inserted, false if updated</returns>
		bool AddOrUpdateMod(Modio::ModInfo ModToAdd)
		{
			if (ModEntries.find(ModToAdd.ModId) == ModEntries.end())
			{
				ModEntries[ModToAdd.ModId] = std::make_shared<Modio::ModCollectionEntry>(ModToAdd);
				return true;
			}
			else
			{
				ModEntries[ModToAdd.ModId]->UpdateModProfile(ModToAdd);
				return false;
			}
		}
		const std::map<Modio::ModID, std::shared_ptr<Modio::ModCollectionEntry>>& Entries()
		{
			return ModEntries;
		}
		Modio::Optional<Modio::ModCollectionEntry&> GetByModID(Modio::ModID ModId) const
		{
			if (ModEntries.count(ModId))
			{
				return *(ModEntries.at(ModId));
			}
			else
			{
				return {};
			}
		}
		bool RemoveMod(Modio::ModID ModId)
		{
			if (ModEntries.count(ModId))
			{
				if (ModEntries.at(ModId)->GetModState() == ModState::UninstallPending)
				{
					ModEntries.erase(ModId);
					return true;
				}
			}
			return false;
		}
		friend void to_json(nlohmann::json& Json, const Modio::ModCollection& Collection)
		{
			std::vector<Modio::ModCollectionEntry> ResolvedEntries;
			for (auto& Mod : Collection.ModEntries)
			{
				ResolvedEntries.push_back(*(Mod.second));
			}
			Json = {Modio::Detail::Constants::JSONKeys::ModCollection, ResolvedEntries};
		}

		friend void from_json(const nlohmann::json& Json, Modio::ModCollection& Collection)
		{
			std::vector<Modio::ModCollectionEntry> LoadedEntries;
			Modio::Detail::ParseSafe(Json, LoadedEntries, Modio::Detail::Constants::JSONKeys::ModCollection);
			for (Modio::ModCollectionEntry& Entry : LoadedEntries)
			{
				Collection.ModEntries.insert_or_assign(Entry.GetID(),
													   std::make_shared<Modio::ModCollectionEntry>(Entry));
			}
		}

	private:
		std::map<Modio::ModID, std::shared_ptr<Modio::ModCollectionEntry>> ModEntries;
	};

	class ModEventLog
	{
	public:
		/// <summary>
		/// Adds a new entry to the log. If a given mod ID has an existing entry, this replaces it.
		/// </summary>
		/// <param name="Entry"></param>
		void AddEntry(Modio::ModManagementEvent Entry)
		{
			InternalData.push_back(std::move(Entry));
		}
		/// <summary>
		/// Updates the error code/status code for a given ModID in the log.
		/// </summary>
		/// <param name="ID">The mod to update</param>
		/// <param name="ec"> the new status code</param>
		void UpdateStatusCode(Modio::ModID ID, Modio::ErrorCode ec);
		/// <summary>
		/// Empties out the log of events
		/// </summary>
		/// <returns>All entries in the log at the time ClearLog was called</returns>
		inline std::vector<Modio::ModManagementEvent> ClearLog()
		{
			auto TmpLog = InternalData;
			InternalData.clear();
			return TmpLog;
		}

	private:
		std::vector<Modio::ModManagementEvent> InternalData;
	};

} // namespace Modio