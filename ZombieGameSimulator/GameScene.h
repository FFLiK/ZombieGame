#pragma once
#include "Scene.h"
#include "Game.h"
#include "Hexagon.h"
#include "Window.h"

class GameScene : public Scene {
public:
	GameScene(Window *win, Game* game);
	~GameScene();
	int Rendering() override;

private:
	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;

	Game* game;
	Window* win;
	std::vector<Hexagon*> path;

	SDL_Texture* timer_title_tex = nullptr;
	SDL_Texture* score_title_tex = nullptr;
	vector<SDL_Texture*> timer_text_tex;
	vector<SDL_Texture*> score_text_tex;

	bool is_moving = false;
};

