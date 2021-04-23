#pragma once

#include "../Engine/State.h"
//TODO@modio-sdk : hacky, need to remove when we work out why asio's defines aren't being set correctly
#include "modio/detail/ModioDefines.h"
#include "modio/core/entities/ModioModInfoList.h"
#include "modio/core/ModioStdTypes.h" //For Modio::Optional


namespace OpenXcom
{
class Window;
class Text;
class TextList;
class TextEdit;
class TextButton;
class ComboBox;
class LayoutGroup;
class Bar;

class OptionsModBrowserState : public State
{
private:
	Window *_window;
	TextEdit* _searchText;
	TextButton* _searchButton;
	Surface* _searchBarPadding;
	LayoutGroup* _searchBar;

	TextList* _modList;
	Surface* _modListScrollPlaceholder;
	LayoutGroup* _modListGroup;
	TextButton* _prevButton;
	TextButton* _nextButton;
	LayoutGroup* _browseNavButtonGroup;
	//May end up being text instead for the label for the bar
	Surface* _queueSeparator;
	Bar* _progress;
	TextButton* _queueButton;
	LayoutGroup* _browseButtonGroup;

	Surface* _browseGroupPadding;
	LayoutGroup* _browseGroup;

	Text* _modDesc;

	TextButton* _subscribeButton;
	TextButton* _detailsButton;
	Surface* _actionButtonSeparator;
	TextButton* _optionsButton;
	TextButton* _backButton;
	LayoutGroup* _actionButtonGroup;

	LayoutGroup* _detailsGroup;


	int _currentSelectionIndex = -1;
	Modio::Optional<Modio::ModInfoList> _currentModResults;
	void UpdateModList();
	void updateModDetails(Modio::ModInfo modDetails);
public:
	OptionsModBrowserState();
	void init() override;

	void think() override;
	/// Handler for showing mod details when one is clicked in the list
	void onModSelected(Action* action);
	/// Handler for clicking the search button
	void onSearchClicked(Action* action);
	void onQueueClicked(Action *action);
	/// Handler for clicking the subscribe button for a mod
	void onSubscribeClicked(Action* action);
	/// Handler for clicking the details button for a mod
	void onDetailsClicked(Action* action);
	/// Handler for clicking the subscribe button for a mod
	void onOptionsClicked(Action* action);
	/// Handler for clicking the subscribe button for a mod
	void onBackClicked(Action* action);
};
}
