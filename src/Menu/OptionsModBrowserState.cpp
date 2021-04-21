#include "OptionsModBrowserState.h"
#include "../Engine/Game.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextList.h"
#include "OptionsModBrowserAuthState.h"
#include "../Interface/Window.h"
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "modio/core/entities/ModioModInfo.h"
#include "modio/ModioSDK.h"
#include "../Interface/Utilities.h"
#include "../Engine/CrossPlatform.h"
#include <map>

void OpenXcom::OptionsModBrowserState::UpdateModList()
{
	if (_currentModResults)
	{
		std::map<Modio::ModID, Modio::ModCollectionEntry> SubscribedMods = Modio::QueryUserSubscriptions();
		_modList->clearList();
		//todo:@modio make these autosize if we can
		textContext Context
		{
			_game->getMod()->getFont("FONT_BIG"),
			_game->getMod()->getFont("FONT_SMALL"),
			_game->getLanguage(),
			35,
			16,
			false
		};
		//TODO:@modio expose to localization
		int ActionColumnWidth = processText("Unsubscribe",Context).getTextWidth();
		_modList->setColumns(2, 120 ,ActionColumnWidth);

		for (const Modio::ModInfo& Info : *_currentModResults)
		{
			_modList->addRow(2, Info.ProfileName.c_str(),"Subscribe");
			std::size_t CurrentRowIndex = _modList->getRows() - 1;
			if (SubscribedMods.count(Info.ModId))
			{
				_modList->setCellText(CurrentRowIndex, 1, "Unsubscribe");
				//_modList->setCellColor(CurrentRowIndex, 1, 127);
			}
			else
			{
				//_modList->setCellText(CurrentRowIndex, 2, "Subscribe");
				//_modList->setCellColor(CurrentRowIndex, 2, 6);
			}
		}
	}
}

void OpenXcom::OptionsModBrowserState::updateModDetails(Modio::ModInfo modDetails)
{
	_modName->setText("Mod Name: " + modDetails.ProfileName);
	_modUpdated->setText("Mod Updated: " + CrossPlatform::timeToString((time_t)modDetails.ProfileDateUpdated).first);
	_modCreated->setText("Mod Creator: " + modDetails.ProfileSubmittedBy.Username);
	_modDesc->setText("Mod Description: " + modDetails.ProfileDescriptionPlaintext);
}

OpenXcom::OptionsModBrowserState::OptionsModBrowserState()
{
	_window = new Window(this, 320, 200);
	_searchText = new TextEdit(this, 240, 16);
	_searchButton = new TextButton(50, 16);
	//making the text list a bit narrower so the arrows are in the main window
	_modList = new TextList(320 - (12 + 11), 100);
	_modName = new Text(320, 16);
	_modUpdated = new Text(320, 16);
	_modCreated = new Text(320, 16);
	_modDesc = new Text(320, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_searchText, "text", "modsMenu");
	add(_searchButton, "button", "optionsMenu");
	add(_modList, "optionLists", "controlsMenu");

	add(_modName, "text", "modsMenu");
	add(_modUpdated, "text", "modsMenu");
	add(_modCreated, "text", "modsMenu");
	add(_modDesc, "text", "modsMenu");
	
	_searchButton->setText("Search");
	_searchButton->autoWidth(100);
	_searchButton->onMouseClick((ActionHandler)&OptionsModBrowserState::onSearchClicked);
	_searchBar = LayoutGroup::Horizontal(320, 16, LayoutParam(_searchText).Proportional(4, 1), LayoutParam(_searchButton).KeepSize());

	_modName->setText("Mod Name:");
	_modUpdated->setText("Mod Updated:");
	_modCreated->setText("Mod Creator:");
	_modDesc->setText("Mod Description:");
	_modDesc->setWordWrap(true);
	//_modList->initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());

	_modList->setBackground(_window);
	_modList->setSelectable(true);
	_modList->onMouseClick((ActionHandler)&OptionsModBrowserState::onModSelected);

	_detailsHeader = LayoutGroup::Horizontal(320, 16,
		LayoutParam(_modName).Proportional(2, 1),
		LayoutParam(_modCreated).Proportional(2, 1),
		LayoutParam(_modUpdated).Proportional(1, 1));

	_details = LayoutGroup::Vertical(320, 100,
		LayoutParam(_detailsHeader).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_modDesc).Proportional(1, 1).OtherAxisStretch());

	add(_detailsHeader);
	add(_details);


	LayoutDriver Driver = LayoutDriver(LayoutDirection::Vertical, _window,
		LayoutParam(_searchBar).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_modList).Proportional(1, 1).KeepSize(),
		LayoutParam(_details).Proportional(1, 1).OtherAxisStretch())
		.Padding(4);
	Driver.ApplyLayout();

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserState::init()
{
	State::init();

	if (!Modio::QueryUserProfile())
	{
		_game->pushState(new OptionsModBrowserAuthState());
	}
	else
	{
		Modio::ListAllModsAsync(Modio::FilterParams(), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods)
		{
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});

	}
}

void OpenXcom::OptionsModBrowserState::think()
{
	State::think();
	
}

void OpenXcom::OptionsModBrowserState::onModSelected(Action* action)
{
	int selectionIndex = _modList->getSelectedRow();
	if (_currentModResults)
	{
		updateModDetails((*_currentModResults)[selectionIndex]);
	}
}

void OpenXcom::OptionsModBrowserState::onSearchClicked(Action* action)
{
	if (!_searchText->getText().empty())
	{
		Modio::ListAllModsAsync(Modio::FilterParams().NameContains(_searchText->getText()), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods)
		{
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});
	}
	else
	{
		Modio::ListAllModsAsync(Modio::FilterParams(), [this](Modio::ErrorCode ec, Modio::Optional<Modio::ModInfoList> Mods)
		{
			if (!ec)
			{
				_currentModResults = Mods;
				UpdateModList();
			}
		});
	}
}

void OpenXcom::OptionsModBrowserState::onSubscribeClicked(Action* action)
{
	int selectionIndex = _modList->getSelectedRow();
	if (_currentModResults)
	{
		Modio::SubscribeToModAsync((*_currentModResults)[selectionIndex].ModId, [](Modio::ErrorCode ec)
		{
			if (ec)
			{
				//error state
			}
		});
	}
}
