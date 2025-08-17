#pragma once
#include "Scene.h"
#include "Game.h"
#include "FinalHumanScene.h"
#include "Input.h"

class FinalHumanScene : public Scene {
public:
	FinalHumanScene(Game* game);
	~FinalHumanScene();
	int Rendering() override;

	bool IsEnd();

private:
	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;

	const int transition_level_delta = 40 * Global::WIN::FRAME_RATE_MULTIPLIER;

	int start_transition_level = 0;
	int end_transition_level = 0;

	SDL_Texture* title_texture = nullptr;
	SDL_Texture* description_texture = nullptr;

	Input* input = nullptr;

	bool is_activated_get_input = false;

	bool is_finished = false;

	Game* game = nullptr;
};

