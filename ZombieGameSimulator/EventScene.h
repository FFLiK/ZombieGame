#pragma once
#include "Scene.h"
#include "Game.h"

class EventScene : public Scene {
public:
	EventScene(Game* game);
	~EventScene();
	int Rendering();

private:
	int EventProcess(Event& evt);
	int NormalProcess();

	Game* game;
};

