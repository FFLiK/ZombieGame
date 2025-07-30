#include "BackgroundScene.h"
#include <Log.h>

BackgroundScene::BackgroundScene() {
	Log::FormattedDebug("BackgroundScene", "Constructor", "Calling constructor of BackgroundScene");
}

BackgroundScene::~BackgroundScene() {
	Log::FormattedDebug("BackgroundScene", "Destructor", "Calling destructor of BackgroundScene");
}

int BackgroundScene::Rendering() {
	return 0;
}


int BackgroundScene::EventProcess(Event& evt) {
	return 0;
}

int BackgroundScene::NormalProcess() {
	return 0;
}