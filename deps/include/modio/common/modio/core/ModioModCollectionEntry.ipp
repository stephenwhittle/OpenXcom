
#ifdef MODIO_SEPARATE_COMPILATION
	#include "modio/core/ModioModCollectionEntry.h"
#endif

#include "modio/core/ModioLogger.h"
#include "modio/detail/ModioJsonHelpers.h"
#include "nlohmann/json.hpp"
#include <algorithm>

namespace Modio
{
	ModCollectionEntry::ModCollectionEntry(ModInfo ProfileData, Modio::filesystem::path CalculatedModPath)
		: ID(ProfileData.ModId),
		  CurrentState(Modio::ModState::InstallationPending),
		  ModProfile(ProfileData),
		  LocalUserSubscriptions(),
		  PathOnDisk(CalculatedModPath) {};

	ModCollectionEntry::ModCollectionEntry(const ModCollectionEntry& Other)
		: ID(Other.ID),
		  CurrentState(Other.CurrentState.load()),
		  ModProfile(Other.ModProfile),
		  LocalUserSubscriptionCount(Other.LocalUserSubscriptionCount.load()),
		  LocalUserSubscriptions(Other.LocalUserSubscriptions),
		  PathOnDisk(Other.PathOnDisk) {};

	void ModCollectionEntry::UpdateModProfile(ModInfo ProfileData)
	{
		// check version in metadata and set pending install if need be
		if (ModProfile.FileInfo.MetadataId != ProfileData.FileInfo.MetadataId)
		{
			SetModState(ModState::UpdatePending);
		}
		ModProfile = ProfileData;
	}

	uint8_t ModCollectionEntry::AddLocalUserSubscription(Modio::Optional<User> User)
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

	uint8_t ModCollectionEntry::RemoveLocalUserSubscription(Modio::Optional<Modio::User> User)
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

	void ModCollectionEntry::SetModState(Modio::ModState NewState)
	{
		CurrentState = NewState;
	}

	void ModCollectionEntry::MarkModNoRetry()
	{
		ShouldNotRetry.store(true);
	}

	void ModCollectionEntry::ClearModNoRetry()
	{
		ShouldNotRetry.store(false);
	}

	bool ModCollectionEntry::ShouldRetry()
	{
		return !ShouldNotRetry.load();
	}

	Modio::ModState ModCollectionEntry::GetModState() const
	{
		return CurrentState;
	}

	Modio::ModID ModCollectionEntry::GetID() const
	{
		return ID;
	}

	Modio::ModInfo ModCollectionEntry::GetModProfile() const
	{
		return ModProfile;
	}

	Modio::filesystem::path ModCollectionEntry::GetPath() const
	{
		return PathOnDisk;
	}

	void ModCollectionEntry::UpdateSizeOnDisk(Modio::FileSize NewSize)
	{
		SizeOnDisk = NewSize;
	}

	void RollbackTransactionImpl(ModCollectionEntry& Entry)
	{
		if (!Entry.RollbackState.has_value())
		{
			Modio::Detail::Logger().Log(Modio::LogLevel::Warning, Modio::LogCategory::ModManagement,
										"Tried to rollback a transaction on a mod in the system collection while no "
										"transaction was occurring!");
			return;
		}
		else
		{
			Entry.CurrentState.store(Entry.RollbackState.take().value());
		}
	}

	void BeginTransactionImpl(ModCollectionEntry& Entry)
	{
		if (Entry.CurrentState == Modio::ModState::Downloading || Entry.CurrentState == Modio::ModState::Extracting ||
			Entry.RollbackState.has_value())
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

	ModCollectionEntry& ModCollectionEntry::operator=(const ModCollectionEntry& Other)
	{
		ID = Other.ID;
		CurrentState.store(Other.CurrentState.load());
		ModProfile = Other.ModProfile;
		LocalUserSubscriptions = Other.LocalUserSubscriptions;
		LocalUserSubscriptionCount.store(Other.LocalUserSubscriptionCount.load());
		PathOnDisk = Other.PathOnDisk;
		return *this;
	};

	void to_json(nlohmann::json& j, const ModCollectionEntry& Entry)
	{
		Modio::ModState EntryState = Entry.CurrentState.load();
		// If the current state is in progress, then we store the previous state. As when loading, the user might
		// not want to start the download/installation during SDK initialization. Instead the progress will be
		// resumed when the mod management loop is enabled
		if (Entry.CurrentState == Modio::ModState::Downloading || Entry.CurrentState == Modio::ModState::Extracting)
		{
			if (Entry.RollbackState.has_value())
			{
				EntryState = *Entry.RollbackState;
			}
			else
			{
				Modio::Detail::Logger().Log(
					Modio::LogLevel::Warning, Modio::LogCategory::ModManagement,
					"Mod {0} is in state Downloading or Extracting without a transaction in progress. Saving state as InstallationPending", Entry.ID);
				EntryState = Modio::ModState::InstallationPending;
			}
		}

		j = nlohmann::json::object(
			{{Modio::Detail::Constants::JSONKeys::ModEntryID, Entry.ID},
			 {Modio::Detail::Constants::JSONKeys::ModEntryProfile, Entry.ModProfile},
			 {Modio::Detail::Constants::JSONKeys::ModEntrySubCount, Entry.LocalUserSubscriptions},
			 {Modio::Detail::Constants::JSONKeys::ModEntryState, EntryState},
			 {Modio::Detail::Constants::JSONKeys::ModSizeOnDisk, Entry.SizeOnDisk},
			 {Modio::Detail::Constants::JSONKeys::ModPathOnDisk, Entry.PathOnDisk.string()}});
	}

	void from_json(const nlohmann::json& j, ModCollectionEntry& Entry)
	{
		Modio::Detail::ParseSafe(j, Entry.ID, Modio::Detail::Constants::JSONKeys::ModEntryID);
		Modio::Detail::ParseSafe(j, Entry.ModProfile, Modio::Detail::Constants::JSONKeys::ModEntryProfile);
		Modio::Detail::ParseSafe(j, Entry.LocalUserSubscriptions, Modio::Detail::Constants::JSONKeys::ModEntrySubCount);
		Modio::Detail::ParseSafe(j, Entry.SizeOnDisk, Modio::Detail::Constants::JSONKeys::ModSizeOnDisk);
		Modio::ModState StateTmp = ModState::InstallationPending;
		Modio::Detail::ParseSafe(j, StateTmp, Modio::Detail::Constants::JSONKeys::ModEntryState);
		Entry.CurrentState.store(StateTmp);
		Modio::Detail::ParseSafe(j, Entry.PathOnDisk, Modio::Detail::Constants::JSONKeys::ModPathOnDisk);
	}

	UserSubscriptionList::UserSubscriptionList(std::vector<Modio::ModID>&& NewIDs)
		: InternalList(std::make_move_iterator(NewIDs.begin()), std::make_move_iterator(NewIDs.end()))
	{}

	UserSubscriptionList::UserSubscriptionList(std::vector<Modio::ModID> NewIDs)
		: InternalList(NewIDs.begin(), NewIDs.end())
	{}

	UserSubscriptionList::UserSubscriptionList() {}

	bool UserSubscriptionList::AddMod(Modio::ModInfo Mod)
	{
		return InternalList.insert(Mod.ModId).second;
	}

	void UserSubscriptionList::RemoveMod(Modio::ModID Mod)
	{
		InternalList.erase(Mod);
	}

	const std::set<Modio::ModID>& UserSubscriptionList::Get() const
	{
		return InternalList;
	}

	std::map<Modio::ModID, Modio::UserSubscriptionList::ChangeType> UserSubscriptionList::CalculateChanges(
		const UserSubscriptionList& Original, const UserSubscriptionList& Updated)
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

	const Modio::ModCollection ModCollection::FilterByUserSubscriptions(
		const UserSubscriptionList& UserSubscriptions) const
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

	bool ModCollection::AddOrUpdateMod(Modio::ModInfo ModToAdd, Modio::filesystem::path CalculatedModPath)
	{
		if (ModEntries.find(ModToAdd.ModId) == ModEntries.end())
		{
			ModEntries[ModToAdd.ModId] = std::make_shared<Modio::ModCollectionEntry>(ModToAdd, CalculatedModPath);
			return true;
		}
		else
		{
			ModEntries[ModToAdd.ModId]->UpdateModProfile(ModToAdd);
			return false;
		}
	}

	const std::map<Modio::ModID, std::shared_ptr<Modio::ModCollectionEntry>>& ModCollection::Entries()
	{
		return ModEntries;
	}

	Modio::Optional<Modio::ModCollectionEntry&> ModCollection::GetByModID(Modio::ModID ModId) const
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

	bool ModCollection::RemoveMod(Modio::ModID ModId)
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

} // namespace Modio