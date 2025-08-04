#include "EventScene.h"
#include "Log.h"

EventScene::EventScene(Game* game) {
	this->game = game;
	Log::FormattedDebug("EventScene", "Constructor", "Calling constructor of EventScene");
}

EventScene::~EventScene() {
	Log::FormattedDebug("EventScene", "Destructor", "Calling destructor of EventScene");
}

int EventScene::Rendering() {
	return 0;
}

int EventScene::ProcessInit() {
    return 0;
}

int EventScene::EventProcess(Event& evt) {
	return 0;
}

int EventScene::NormalProcess() {
	return 0;
}
