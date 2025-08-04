#pragma once
#include "Scene.h"
#include "Game.h"

class BackgroundScene : public Scene {
public:
	BackgroundScene();
	~BackgroundScene();
	int Rendering() override;
	
private:
	SDL_Texture* title_tex = nullptr;
	SDL_Texture* version_tex = nullptr;
	SDL_Texture* developer_tex = nullptr;

	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;
};

