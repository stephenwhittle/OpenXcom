#include "OptionsModBrowserAuthState.h"
#include "../Interface/Window.h"
#include "../Interface/TextEdit.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/LayoutDriver.h"
#include "../Interface/LayoutGroup.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/FileMap.h"
#include "../Engine/Options.h"
#include "../Engine/Game.h"
#include "../Menu/ErrorMessageState.h"
#include "../Mod/RuleInterface.h"
#include "../Mod/Mod.h"
#include "modio/ModioSDK.h"

void OpenXcom::OptionsModBrowserAuthState::submitEmailClick(Action* action)
{
	Modio::RequestEmailAuthCodeAsync(Modio::EmailAddress(_emailAddrInput->getText()), [this](Modio::ErrorCode ec) {
		if (ec)
		{
			_game->pushState(new ErrorMessageState((std::string("mod.io SDK auth failure: ") + ec.message()).c_str(), _palette, _game->getMod()->getInterface("errorMessages")->getElement("geoscapeColor")->color, "BACK01.SCR", _game->getMod()->getInterface("errorMessages")->getElement("geoscapePalette")->color));
		}
	});
}

void OpenXcom::OptionsModBrowserAuthState::submitAuthCodeClick(Action* action)
{
	Modio::AuthenticateUserEmailAsync(Modio::EmailAuthCode(_authCodeInput->getText()), [this](Modio::ErrorCode ec) {
		if (ec)
		{
			_game->pushState(new ErrorMessageState((std::string("mod.io SDK auth failure: ") + ec.message()).c_str(), _palette, _game->getMod()->getInterface("errorMessages")->getElement("geoscapeColor")->color, "BACK01.SCR", _game->getMod()->getInterface("errorMessages")->getElement("geoscapePalette")->color));
		}
		else
		{
			_game->popState();
		}
	});
}

void OpenXcom::OptionsModBrowserAuthState::think()
{
	State::think();
	Modio::RunPendingHandlers();
}

OpenXcom::OptionsModBrowserAuthState::OptionsModBrowserAuthState()
{
	_window = new Window(this, 320, 200, 0, 0, POPUP_VERTICAL);
	_modioLogo = new InteractiveSurface(100, 100);
	_authRequiredText = new Text(240, 100);
	_emailAddrInput = new TextEdit(this, 240, 16);
	_sendCodeRequestBtn = new TextButton(50, 16);
	_authCodeInput = new TextEdit(this, 240, 16);
	_submitAuthBtn = new TextButton(50, 16);

	setInterface("optionsMenu");

	add(_window, "window", "optionsMenu");
	
	add(_authRequiredText, "text", "optionsMenu");
	add(_emailAddrInput, "text1", "optionsMenu");
	add(_sendCodeRequestBtn, "button", "optionsMenu");
	add(_authCodeInput, "text1", "optionsMenu");
	add(_submitAuthBtn, "button", "optionsMenu");

	_authRequiredText->setText("Please enter the email address for your mod.io account to authenticate and manage your OpenXcom mod subscriptions");
	_authRequiredText->setColor(138);
	_authRequiredText->setWordWrap(true);
	_authRequiredText->setAlign(ALIGN_CENTER);

	_emailAddrInput->setTooltip("Email Address");
	_emailAddrInput->setText("Email Address");
	_sendCodeRequestBtn->setText("Submit code request");
	_sendCodeRequestBtn->autoWidth(300);
	_sendCodeRequestBtn->onMouseClick((ActionHandler)&OptionsModBrowserAuthState::submitEmailClick);
	_authCodeInput->setTooltip("Enter the code sent to your email here");
	_authCodeInput->setText("Authentication Code");
	_submitAuthBtn->setText("Submit authentication code");
	_submitAuthBtn->autoWidth(300);
	_submitAuthBtn->onMouseClick((ActionHandler)&OptionsModBrowserAuthState::submitAuthCodeClick);
	//Todo: set height of text edits to the maximum height of the font in question so they are centered vertically
	_emailWidgetGroup = LayoutGroup::Horizontal(320, 16, LayoutParam(_emailAddrInput).Proportional(4, 1).OtherAxisStretch(), LayoutParam(_sendCodeRequestBtn).KeepSize());
	_codeWidgetGroup = LayoutGroup::Horizontal(320, 16, LayoutParam(_authCodeInput).Proportional(4, 1).OtherAxisCenter(), LayoutParam(_submitAuthBtn).KeepSize());


	LayoutDriver d = LayoutDriver(LayoutDirection::Vertical, _window,
		LayoutParam(_modioLogo).Proportional(1, 3).OtherAxisStretch(),
		LayoutParam(_authRequiredText).Proportional(1, 1).OtherAxisStretch(),
		LayoutParam(_emailWidgetGroup).Absolute(1,16).OtherAxisStretch(),
		LayoutParam(_codeWidgetGroup).Absolute(1,16).OtherAxisStretch())
		.Padding(4);

	d.ApplyLayout();

	_modioLogo->loadImage(FileMap::getFilePath("modiocolor.png"));

	centerAllSurfaces();
}
