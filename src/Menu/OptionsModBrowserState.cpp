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
#include "modio/ModioSDK.h"

OpenXcom::OptionsModBrowserState::OptionsModBrowserState()
{
	_window = new Window(this, 320, 200);
	_searchText = new TextEdit(this, 240, 16);
	_searchButton = new TextButton(50, 16);
	_searchBar = LayoutGroup::Horizontal(320, 16, LayoutParam(_searchText).Proportional(4, 1), LayoutParam(_searchButton).Proportional(1, 1));
	_modList = new TextList(320, 100);
	_modList->initText(_game->getMod()->getFont("FONT_BIG"), _game->getMod()->getFont("FONT_SMALL"), _game->getLanguage());

	LayoutDriver Driver = LayoutDriver(LayoutDirection::Vertical, _window, LayoutParam(_searchBar).Absolute(1, 16).OtherAxisStretch(), LayoutParam(_modList).Proportional(1,1).OtherAxisStretch())
		.Padding(4);
	Driver.ApplyLayout();

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_searchText, "text", "modsMenu");
	add(_searchButton, "button", "optionsMenu");
	add(_modList, "optionLists", "controlsMenu");

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserState::init()
{
	State::init();

	if (!Modio::QueryUserProfile().has_value())
	{
		_game->pushState(new OptionsModBrowserAuthState());
	}

}

void OpenXcom::OptionsModBrowserState::think()
{
	State::think();
}
