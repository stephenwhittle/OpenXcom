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
#include <map>

void OpenXcom::OptionsModBrowserState::UpdateModList()
{
	if (_currentModResults)
	{
		std::map<Modio::ModID, Modio::ModCollectionEntry> SubscribedMods = Modio::QueryUserSubscriptions();
		_modList->clearList();
		//todo:@modio make these autosize if we can
		_modList->setColumns(3, 80, 200, 35);
		for (const Modio::ModInfo& Info : *_currentModResults)
		{
			_modList->addRow(3, Info.ProfileName.c_str(), Info.ProfileDescriptionPlaintext.c_str(), "Subscribe");
			std::size_t CurrentRowIndex = _modList->getRows() - 1;
			if (SubscribedMods.count(Info.ModId))
			{
				_modList->setCellText(CurrentRowIndex, 2, "Unsubscribe");
				_modList->setCellColor(CurrentRowIndex, 2, 127);
			}
			else
			{
				//_modList->setCellText(CurrentRowIndex, 2, "Subscribe");
				_modList->setCellColor(CurrentRowIndex, 2, 6);
			}
		}
	}
}

OpenXcom::OptionsModBrowserState::OptionsModBrowserState()
{
	_window = new Window(this, 320, 200);
	_searchText = new TextEdit(this, 240, 16);
	_searchButton = new TextButton(50, 16);
	_modList = new TextList(320, 100);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_searchText, "text", "modsMenu");
	add(_searchButton, "button", "optionsMenu");
	add(_modList, "optionLists", "controlsMenu");

	_searchButton->setText("Search");
	_searchButton->autoWidth(100);
	_searchBar = LayoutGroup::Horizontal(320, 16, LayoutParam(_searchText).Proportional(4, 1), LayoutParam(_searchButton).KeepSize());
	
	//_modList->initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());

	LayoutDriver Driver = LayoutDriver(LayoutDirection::Vertical, _window, LayoutParam(_searchBar).Absolute(1, 16).OtherAxisStretch(), LayoutParam(_modList).Proportional(1,1).OtherAxisStretch())
		.Padding(4);
	Driver.ApplyLayout();

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserState::init()
{
	State::init();

	if (!Modio::QueryUserProfile().has_value())
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
	Modio::RunPendingHandlers();
}
