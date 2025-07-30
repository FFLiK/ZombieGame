#pragma once
#include "Scene.h"
#include "Game.h"

class GameScene : public Scene {
public:
	GameScene(Game* game);
	~GameScene();
	int Rendering();

private:
	int EventProcess(Event& evt);
	int NormalProcess();

	Game* game;
};

