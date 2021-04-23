//use a fixed number of widgets, show and hide as we paginate through them
#pragma once

#include "../Engine/State.h"

#include "modio/detail/ModioDefines.h"

#include "modio/core/ModioStdTypes.h"

namespace OpenXcom
{
class Window;
class Text;
class Bar;
class TextList;
class TextButton;
class Surface;
class LayoutGroup;

class OptionsModBrowserQueueState : public State
{
	Window *_window;
	Modio::ModID _currentModID;
	Text *_currentDownloadLabel;
	Text *_currentDownloadName;
	Text *_currentDownloadStatus;
	Bar *_currentDownloadProgress;
	LayoutGroup *_downloadGroup;

	TextList *_pendingDownloadList;
	LayoutGroup *_queueGroup;

	Surface *_buttonSpacer;
	TextButton *_backButton;
	LayoutGroup *_buttonGroup;
	void onBackClicked(Action *action);

  public:
	OptionsModBrowserQueueState();
	void think() override;
};

}
