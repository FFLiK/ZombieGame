#pragma once
#include "Scene.h"
#include "Game.h"

class BackgroundScene : public Scene {
public:
	BackgroundScene(Game *game);
	~BackgroundScene();
	int Rendering();
	
private:
	int EventProcess(Event& evt);
	int NormalProcess();

	Game *game;

	std::vector<Hexagon*> path;
};

