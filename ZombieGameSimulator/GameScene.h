#pragma once
#include "Scene.h"
#include "Game.h"
#include "Hexagon.h"
#include "Window.h"

class GameScene : public Scene {
public:
	GameScene(Window *win, Game* game);
	~GameScene();
	int Rendering();

private:
	int EventProcess(Event& evt);
	int NormalProcess();

	Game* game;
	Window* win;
	std::vector<Hexagon*> path;

	bool is_moving = false;
};

