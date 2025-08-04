#pragma once
#include "Scene.h"
#include "Game.h"

class EventScene : public Scene {
public:
	EventScene(Game* game);
	~EventScene();
	int Rendering() override;

private:
	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;

	Game* game = nullptr;
};

