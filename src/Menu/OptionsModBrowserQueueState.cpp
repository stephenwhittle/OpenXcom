#include "OptionsModBrowserQueueState.h"
#include "../Engine/Game.h"
#include "../Engine/Surface.h"
#include "../Interface/Bar.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextList.h"
#include "../Interface/Window.h"

#include "modio/ModioSDK.h"

OpenXcom::OptionsModBrowserQueueState::OptionsModBrowserQueueState()
{
	_window = new Window(this, 320, 200);
	_currentDownloadLabel = new Text(320, 200);
	_currentDownloadName = new Text(320, 200);
	_currentDownloadProgress = new Bar(320, 200);
	_currentDownloadStatus = new Text(320, 200);
	_pendingDownloadList = new TextList(320, 200);
	_buttonSpacer = new Surface(1, 1);
	_backButton = new TextButton(50, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_currentDownloadLabel, "text", "modsMenu");
	add(_currentDownloadName, "text", "modsMenu");
	add(_currentDownloadProgress);
	add(_currentDownloadStatus, "text", "modsMenu");
	add(_pendingDownloadList, "optionLists", "controlsMenu");
	add(_buttonSpacer);
	add(_backButton, "button", "optionsMenu");

	_currentDownloadLabel->setText("Current Download");
	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserQueueState::onBackClicked);

	_pendingDownloadList->setColumns(2, 100, 100);

	_downloadGroup = LayoutGroup::Horizontal(320, 200,
											 LayoutParam(_currentDownloadName).Proportional(2, 1).OtherAxisStretch(),
											 LayoutParam(_currentDownloadProgress).Proportional(1, 1).OtherAxisStretch(),
											 LayoutParam(_currentDownloadStatus).Proportional(1, 1).OtherAxisStretch());
	_queueGroup = LayoutGroup::Vertical(320, 200,
										LayoutParam(_currentDownloadLabel).Absolute(1, 16).OtherAxisStretch(),
										LayoutParam(_downloadGroup).Absolute(1, 16).OtherAxisStretch(),
										LayoutParam(_pendingDownloadList).Proportional(1, 1).OtherAxisStretch());

	_buttonGroup = LayoutGroup::Vertical(320, 200,
										 LayoutParam(_buttonSpacer).Proportional(1, 1).OtherAxisStretch(),
										 LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());

	LayoutDriver topLevelLayout = LayoutDriver(LayoutDirection::Horizontal, _window,
											   LayoutParam(_queueGroup).Proportional(4, 1).OtherAxisStretch(),
											   LayoutParam(_buttonGroup).Proportional(1, 1).OtherAxisStretch())
									  .Padding(4);
	topLevelLayout.ApplyLayout();

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserQueueState::think()
{
	State::think();
	bool bUpdateQueuedMods = false;

	std::map<Modio::ModID, Modio::ModCollectionEntry> CurrentMods = Modio::QueryUserSubscriptions();
	Modio::Optional<Modio::ModProgressInfo> CurrentProgress = Modio::QueryCurrentModUpdate();
	if (CurrentProgress.has_value())
	{
		Modio::ModCollectionEntry &currentMod = CurrentMods[CurrentProgress->ID];
		_currentDownloadName->setText(currentMod.GetModProfile().ProfileName);
		switch(currentMod.GetModState())
		{
		case Modio::ModState::Downloading:
			_currentDownloadProgress->setValue(CurrentProgress->CurrentlyDownloadedBytes / (float)CurrentProgress->TotalDownloadSize);
			_currentDownloadStatus->setText("Downloading");
			break;
		case Modio::ModState::Extracting:
			_currentDownloadProgress->setValue(CurrentProgress->CurrentlyExtractedBytes / (float)CurrentProgress->TotalExtractedSizeOnDisk);
			_currentDownloadStatus->setText("Extracting");
			break;

		}

		//Still more mods to download, current one has changed
		if (CurrentProgress->ID != _currentModID)
		{
			bUpdateQueuedMods = true;
		}

		_currentModID = CurrentProgress->ID;

	}
	else
	{
		//Transition from mod download in progress -> no more downloads
		if (_currentModID != Modio::ModID(-1))
		{
			_currentModID = Modio::ModID(-1);
			bUpdateQueuedMods = true;
		}
		
	}

	if (bUpdateQueuedMods)
	{
		_pendingDownloadList->clearList();

		//Iterate through current subs, rendering failed ones and pending ones
		for (std::pair<Modio::ModID, Modio::ModCollectionEntry> mod : CurrentMods)
		{
			if (mod.second.GetModState() != Modio::ModState::Installed && mod.second.GetModState() != Modio::ModState::UninstallPending)
			{
				_pendingDownloadList->addRow(2, mod.second.GetModProfile().ProfileName, "Pending");
			}
		}
	}
	
}

void OpenXcom::OptionsModBrowserQueueState::onBackClicked(Action *action)
{
	_game->popState();
}
