#include "OptionsModBrowserState.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Game.h"
#include "../Interface/Bar.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextList.h"
#include "../Interface/Utilities.h"
#include "../Interface/Window.h"
#include "../Mod/Mod.h"
#include "OptionsModBrowserAuthState.h"
#include "OptionsModBrowserDetailsState.h"
#include "OptionsModBrowserQueueState.h"
#include "OptionsModBrowserUserConfigState.h"
#include "modio/core/entities/ModioModInfo.h"
#include <map>

#include "modio/ModioSDK.h"
void OpenXcom::OptionsModBrowserState::UpdateModList()
{
	if (_currentModResults)
	{
		std::map<Modio::ModID, Modio::ModCollectionEntry> SubscribedMods = Modio::QueryUserSubscriptions();
		_modList->clearList();

		//Workaround - forces the selector Surface to be resized based on the new size driven by the LayoutDriver
		_modList->setSmall();

		//todo:@modio make these autosize if we can
		_modList->setColumns(1, 320 - (12 + 11));

		for (const Modio::ModInfo &Info : *_currentModResults)
		{
			_modList->addRow(1, Info.ProfileName.c_str());
			std::size_t CurrentRowIndex = _modList->getRows() - 1;
			if (SubscribedMods.count(Info.ModId))
			{
				_modList->setCellColor(CurrentRowIndex, 0, 6);
			}
		}
	}
}

void OpenXcom::OptionsModBrowserState::updateModDetails(Modio::ModInfo modDetails)
{
	_modDesc->setText(modDetails.ProfileDescriptionPlaintext);
	if (Modio::QueryUserSubscriptions().count(modDetails.ModId))
	{
		_subscribeButton->setText("Unsubscribe");
		_subscribeButtonAction = SubscribeButtonMode::Unsubscribe;
	}
	else
	{
		_subscribeButton->setText("Subscribe");
		_subscribeButtonAction = SubscribeButtonMode::Subscribe;
	}
}

OpenXcom::OptionsModBrowserState::OptionsModBrowserState()
{
	_window = new Window(this, 320, 200);
	_searchText = new TextEdit(this, 240, 16);
	_searchButton = new TextButton(50, 16);
	_searchBarPadding = new Surface(1, 1);
	//making the text list a bit narrower so the arrows are in the main window
	_modList = new TextList(320 - (12 + 11), 100);
	_modListScrollPlaceholder = new Surface(13, 16);
	_prevButton = new TextButton(50, 16);
	_nextButton = new TextButton(50, 16);
	_queueSeparator = new Surface(50, 16);
	_progress = new Bar(50, 16);
	_queueButton = new TextButton(50, 16);
	_browseGroupPadding = new Surface(1, 1);

	_modDesc = new Text(320, 16);

	_subscribeButton = new TextButton(50, 16);
	_detailsButton = new TextButton(50, 16);
	_actionButtonSeparator = new Surface(50, 16);
	_optionsButton = new TextButton(50, 16);
	_backButton = new TextButton(50, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_searchText, "text", "modsMenu");
	add(_searchButton, "button", "optionsMenu");
	add(_searchBarPadding);

	add(_modList, "optionLists", "controlsMenu");
	add(_modListScrollPlaceholder);

	add(_prevButton, "button", "optionsMenu");
	add(_nextButton, "button", "optionsMenu");
	add(_queueSeparator);
	add(_progress);
	add(_queueButton, "button", "optionsMenu");
	add(_browseGroupPadding);

	add(_modDesc, "text", "modsMenu");

	add(_subscribeButton, "button", "optionsMenu");
	add(_detailsButton, "button", "optionsMenu");
	add(_actionButtonSeparator);
	add(_optionsButton, "button", "optionsMenu");
	add(_backButton, "button", "optionsMenu");

	_searchButton->setText("Search");
	_searchButton->autoWidth(100);
	_searchButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onSearchClicked);

	_searchBar = LayoutGroup::Horizontal(320, 16, LayoutParam(_searchText).Proportional(4, 1), LayoutParam(_searchButton).Proportional(1, 1));

	_prevButton->setText("Prev");
	_nextButton->setText("Next");
	_progress->setHidden(true);
	_queueButton->setText("0 in queue");
	_queueButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onQueueClicked);

	_modList->setBackground(_window);
	_modList->setSelectable(true);
	_modList->onMouseClick((ActionHandler)&OptionsModBrowserState::onModSelected);

	_modListGroup = LayoutGroup::Horizontal(320, 100,
											LayoutParam(_modList).Proportional(1, 1).OtherAxisStretch(),
											LayoutParam(_modListScrollPlaceholder).Absolute(23, 1).OtherAxisStretch());

	_browseNavButtonGroup = LayoutGroup::Horizontal(320, 100,
													LayoutParam(_prevButton).Proportional(1, 1).OtherAxisStretch(),
													LayoutParam(_nextButton).Proportional(1, 1).OtherAxisStretch());

	_browseButtonGroup = LayoutGroup::Vertical(320, 100,
											   LayoutParam(_browseNavButtonGroup).Absolute(1, 16).OtherAxisStretch(),
											   LayoutParam(_queueSeparator).Proportional(1, 1).OtherAxisStretch(),
											   LayoutParam(_progress).Absolute(1, 16).OtherAxisStretch(),
											   LayoutParam(_queueButton).Absolute(1, 16).OtherAxisStretch());

	_browseGroup = LayoutGroup::Horizontal(320, 100,
										   LayoutParam(_modListGroup).Proportional(4, 1).OtherAxisStretch(),
										   LayoutParam(_browseButtonGroup).Proportional(1, 1).OtherAxisStretch());

	_modDesc->setWordWrap(true);
	_modDesc->setScrollable(true);
	_subscribeButton->setText("Subscribe");
	_subscribeButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onSubscribeClicked);

	_detailsButton->setText("Details");
	_detailsButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onDetailsClicked);

	_optionsButton->setText("User Config");
	_optionsButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onOptionsClicked);

	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onBackClicked);

	_actionButtonGroup = LayoutGroup::Vertical(320, 100,
											   LayoutParam(_subscribeButton).Absolute(1, 16).OtherAxisStretch(),
											   LayoutParam(_detailsButton).Absolute(1, 16).OtherAxisStretch(),
											   LayoutParam(_actionButtonSeparator).Proportional(1, 1).OtherAxisStretch(),
											   LayoutParam(_optionsButton).Absolute(1, 16).OtherAxisStretch(),
											   LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());

	_detailsGroup = LayoutGroup::Horizontal(320, 100,
											LayoutParam(_modDesc).Proportional(4, 1).OtherAxisStretch(),
											LayoutParam(_actionButtonGroup).Proportional(1, 1).OtherAxisStretch());

	LayoutDriver Driver = LayoutDriver(LayoutDirection::Vertical, _window,
									   LayoutParam(_searchBar).Absolute(1, 16).OtherAxisStretch(),
									   LayoutParam(_searchBarPadding).Absolute(1, 8).OtherAxisStretch(),
									   LayoutParam(_browseGroup).Proportional(1, 1).OtherAxisStretch(),
									   LayoutParam(_browseGroupPadding).Absolute(1, 8).OtherAxisStretch(),
									   LayoutParam(_detailsGroup).Proportional(1, 1).OtherAxisStretch())
							  .Padding(4);
	Driver.ApplyLayout();

	//Workaround - forces the selector Surface to be resized based on the new size driven by the LayoutDriver
	_modList->setSmall();
	centerAllSurfaces();
}

OpenXcom::OptionsModBrowserState::~OptionsModBrowserState()
{
}

void OpenXcom::OptionsModBrowserState::init()
{
	State::init();

	if (!Modio::QueryUserProfile())
	{
		_game->pushState(new OptionsModBrowserAuthState());
	}
	else if (!_currentModResults)
	{
		Modio::ListAllModsAsync(Modio::FilterParams(), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods) {
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});
	}
	else
	{
		UpdateModList();
	}
}

void OpenXcom::OptionsModBrowserState::think()
{
	State::think();
}

void OpenXcom::OptionsModBrowserState::onModSelected(Action *action)
{
	_currentSelectionIndex = _modList->getSelectedRow();
	if (_currentModResults && _currentSelectionIndex >= 0)
	{
		updateModDetails((*_currentModResults)[_currentSelectionIndex]);
	}
}

void OpenXcom::OptionsModBrowserState::onSearchClicked(Action *action)
{
	if (!_searchText->getText().empty())
	{
		Modio::ListAllModsAsync(Modio::FilterParams().NameContains(_searchText->getText()), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods) {
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});
	}
	else
	{
		Modio::ListAllModsAsync(Modio::FilterParams(), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods) {
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});
	}
}

void OpenXcom::OptionsModBrowserState::onQueueClicked(Action *action)
{
	_game->pushState(new OptionsModBrowserQueueState());
}

void OpenXcom::OptionsModBrowserState::onSubscribeClicked(Action *action)
{
	if (_currentModResults && _currentSelectionIndex >= 0)
	{
		if (_subscribeButtonAction == SubscribeButtonMode::Subscribe)
		{
			Modio::SubscribeToModAsync((*_currentModResults)[_currentSelectionIndex].ModId, [this](Modio::ErrorCode ec) {
				if (!ec)
				{
					UpdateModList();
					updateModDetails((*_currentModResults)[_currentSelectionIndex]);
				}
			});
		}
		else
		{
			Modio::UnsubscribeFromModAsync((*_currentModResults)[_currentSelectionIndex].ModId, [this](Modio::ErrorCode ec) {
				if (!ec)
				{
					UpdateModList();
					updateModDetails((*_currentModResults)[_currentSelectionIndex]);
				}
			});
		}
	}
}

void OpenXcom::OptionsModBrowserState::onDetailsClicked(Action *action)
{

	if (_currentModResults && _currentSelectionIndex >= 0)
	{
		_game->pushState(new OptionsModBrowserDetailsState((*_currentModResults)[_currentSelectionIndex]));
	}
}

void OpenXcom::OptionsModBrowserState::onOptionsClicked(Action *action)
{
	_game->pushState(new OptionsModBrowserUserConfigState());
}

void OpenXcom::OptionsModBrowserState::onBackClicked(Action *action)
{
	_game->popState();
}
