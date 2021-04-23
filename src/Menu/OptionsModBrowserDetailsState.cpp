#include "OptionsModBrowserDetailsState.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/LayoutGroup.h"
#include "../Interface/LayoutDriver.h"
#include "../Engine/Game.h"

OpenXcom::OptionsModBrowserDetailsState::OptionsModBrowserDetailsState()
{
	_window = new Window(this, 320, 200);
	_modName = new Text(320, 16);
	_modUpdated = new Text(320, 16);

	_modCreator = new Text(320, 16);
	_modRating = new Text(320, 16);

	_modDescription = new Text(320, 16);

	_subscribeButton = new TextButton(50, 16);
	_buttonSeparator = new Surface(1, 1);
	_backButton = new TextButton(50, 16);


	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	add(_modName, "text", "modsMenu");
	add(_modUpdated, "text", "modsMenu");

	add(_modCreator, "text", "modsMenu");
	add(_modRating, "text", "modsMenu");

	add(_modDescription, "text", "modsMenu");

	add(_subscribeButton, "button", "optionsMenu");
	add(_buttonSeparator);
	add(_backButton, "button", "optionsMenu");



	_modName->setText("Mod Name: ");
	_modUpdated->setText("Last Updated: ");
	_modCreator->setText("Mod Uploader: ");
	_modRating->setText("Mod Rating: ");
	_modDescription->setText("Mod Description: ");
	_subscribeButton->setText("Subscribe");
	_backButton->setText("Back");
	_backButton->onMouseClick((ActionHandler)&OptionsModBrowserDetailsState::onBackButtonClicked);

	_headingRow1 = LayoutGroup::Horizontal(320, 100,
		LayoutParam(_modName).Proportional(1,1).OtherAxisStretch(),
		LayoutParam(_modUpdated).Proportional(1,1).OtherAxisStretch());
	_headingRow2 = LayoutGroup::Horizontal(320, 100,
		LayoutParam(_modCreator).Proportional(1, 1).OtherAxisStretch(),
		LayoutParam(_modRating).Proportional(1, 1).OtherAxisStretch());
	_textGroup = LayoutGroup::Vertical(320, 100,
		LayoutParam(_headingRow1).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_headingRow2).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_modDescription).Proportional(1, 1).OtherAxisStretch());
	_buttonGroup = LayoutGroup::Vertical(320, 100,
		LayoutParam(_subscribeButton).Absolute(1, 16).OtherAxisStretch(),
		LayoutParam(_buttonSeparator).Proportional(1, 1).OtherAxisStretch(),
		LayoutParam(_backButton).Absolute(1, 16).OtherAxisStretch());


	LayoutDriver topLevelLayout = LayoutDriver(LayoutDirection::Horizontal, _window,
		LayoutParam(_textGroup).Proportional(4, 1).OtherAxisStretch(),
		LayoutParam(_buttonGroup).Proportional(1, 1).OtherAxisStretch()
	).Padding(4);
	topLevelLayout.ApplyLayout();

	centerAllSurfaces();
}

void OpenXcom::OptionsModBrowserDetailsState::onSubscribeButtonClicked(Action* Action)
{

}

void OpenXcom::OptionsModBrowserDetailsState::onBackButtonClicked(Action* Action)
{
	_game->popState();
}
