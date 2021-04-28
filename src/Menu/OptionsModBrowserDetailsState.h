#pragma once

#include "../Engine/State.h"
#include "modio/core/entities/ModioModInfo.h"

namespace OpenXcom
{
class Window;
class Text;
class LayoutGroup;
class TextButton;

/// @brief State showing a detailed view of a specific mod
class OptionsModBrowserDetailsState : public State
{
	Window *_window;
	Text *_modName;
	Text *_modUpdated;
	LayoutGroup *_headingRow1;
	Text *_modCreator;
	Text *_modRating;
	LayoutGroup *_headingRow2;
	Text *_modDescription;
	LayoutGroup *_textGroup;

	TextButton *_subscribeButton;
	Surface *_buttonSeparator;
	TextButton *_backButton;
	LayoutGroup *_buttonGroup;
	//Handles subscription button click
	void onSubscribeButtonClicked(Action *Action);
	//Handles back button click
	void onBackButtonClicked(Action *Action);
	Modio::ModInfo _modToDisplay;

  public:
	OptionsModBrowserDetailsState(Modio::ModInfo ModToDisplay);
};

}
