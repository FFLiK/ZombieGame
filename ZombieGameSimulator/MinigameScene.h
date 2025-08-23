#pragma once
#include "Scene.h"
#include "Game.h"
#include "EventScene.h"
#include "PdfViewer.h"
#include "Input.h"

class MinigameScene : public Scene {
public:
	MinigameScene(Game* game);
	~MinigameScene();
	int Rendering() override;

	bool IsEnd();

private:
	int ProcessInit() override;
	int EventProcess(Event& evt) override;
	int NormalProcess() override;

	const int transition_level_delta = 40 * Global::WIN::FRAME_RATE_MULTIPLIER;

	int start_transition_level = 0;
	int end_transition_level = 0;
	int title_transition_level = 0;
	int pdf_start_transition_level = 0;
	int pdf_end_transition_level = 0;
	int pdf_delay_transition_level = 0;

	PdfViewer* pdf_viewer = nullptr;
	SDL_Texture* title_texture = nullptr;
	SDL_Texture* pdf_texture = nullptr;

	std::vector<RouletteBoard> roulette_board;

	Input* input = nullptr;

	bool is_activated_get_input = false;

	int roulette_active_num = -1;
	int roulette_spin_speed = 7;
	int roulette_angle = 0;

	bool is_finished = false;

	Game* game = nullptr;
};

