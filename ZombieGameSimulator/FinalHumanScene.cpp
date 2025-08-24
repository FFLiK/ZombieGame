#include "FinalHumanScene.h"
#include "FinalHumanScene.h"
#include "Resources.h"
#include "Texture.h"
#include "Log.h"
#include "Global.h"
#include <vector>
#include <string>
using namespace std;

FinalHumanScene::FinalHumanScene(Game* game) {
	this->game = game;
	this->is_finished = false;
	this->input = nullptr;
	this->is_activated_get_input = false;
}

FinalHumanScene::~FinalHumanScene() {
	if (this->title_texture) {
		SDL_DestroyTexture(this->title_texture);
		this->title_texture = nullptr;
	}
	if (this->description_texture) {
		SDL_DestroyTexture(this->description_texture);
		this->description_texture = nullptr;
	}
	if (this->input) {
		delete this->input;
		this->input = nullptr;
	}
}

int FinalHumanScene::Rendering() {

	double fraction;

	fraction = (double)this->start_transition_level / (double)this->transition_level_delta;
	if (this->end_transition_level) {
		fraction = (double)this->end_transition_level / (double)this->transition_level_delta;
		fraction = 1.0 - fraction;
	}

	int background_alpha = 150 * fraction;

	SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(this->ren, 0, 0, 0, background_alpha);
	SDL_RenderFillRect(this->ren, nullptr);

	if (this->title_texture) {
		// title -> description -> input 순으로 20 * SIZE_MULTIPLIER 만큼 간격을 두고 가운데에 출력
		SDL_Rect title_src, title_dst;
		SDL_Rect description_src, description_dst;
		SDL_Rect input_src, input_dst;
		SDL_QueryTexture(this->title_texture, NULL, NULL, &title_src.w, &title_src.h);
		SDL_QueryTexture(this->description_texture, NULL, NULL, &description_src.w, &description_src.h);
		SDL_QueryTexture(this->input->GetTexture(), NULL, NULL, &input_src.w, &input_src.h);

		SDL_SetTextureBlendMode(this->title_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(this->title_texture, 255 * fraction);
		title_src.x = 0;
		title_src.y = 0;
		title_dst.x = (Global::WIN::SCREEN_WIDTH - title_src.w) / 2;
		title_dst.y = Global::WIN::SCREEN_HEIGHT_HALF - 20 * Global::WIN::SIZE_MULTIPLIER - title_src.h - description_src.h / 2;
		title_dst.w = title_src.w;
		title_dst.h = title_src.h;
		SDL_RenderCopy(this->ren, this->title_texture, &title_src, &title_dst);

		SDL_SetTextureBlendMode(this->description_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(this->description_texture, 255 * fraction);
		description_src.x = 0;
		description_src.y = 0;
		description_dst.x = (Global::WIN::SCREEN_WIDTH - description_src.w) / 2;
		description_dst.y = Global::WIN::SCREEN_HEIGHT_HALF - description_src.h / 2;
		description_dst.w = description_src.w;
		description_dst.h = description_src.h;
		SDL_RenderCopy(this->ren, this->description_texture, &description_src, &description_dst);

		SDL_SetTextureBlendMode(this->input->GetTexture(), SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(this->input->GetTexture(), 255 * fraction);
		input_src.x = 0;
		input_src.y = 0;
		input_dst.x =(Global::WIN::SCREEN_WIDTH - input_src.w) / 2; 
		input_dst.y = Global::WIN::SCREEN_HEIGHT_HALF + 20 * Global::WIN::SIZE_MULTIPLIER + description_src.h / 2;
		input_dst.w = input_src.w;
		input_dst.h = input_src.h;
		SDL_RenderCopy(this->ren, this->input->GetTexture(), &input_src, &input_dst);
	}

	if (this->start_transition_level < this->transition_level_delta) {
		this->start_transition_level++;
	}
	else if (this->is_activated_get_input) {

	}
	else if (this->end_transition_level < this->transition_level_delta) {
		this->end_transition_level++;
	}
	else {
		this->is_finished = true;
	}
	return 0;
}

bool FinalHumanScene::IsEnd() {
	return this->is_finished;
}

int FinalHumanScene::ProcessInit() {
	this->start_transition_level = 0;
	this->end_transition_level = 0;

	this->is_activated_get_input = true;

	string title = "Team " + std::to_string(this->game->final_player->GetIndex() + 1) + " is the Ultimate Survivor";
	this->title_texture = LoadText(title.c_str(), this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);

	string description = "Team " + std::to_string(this->game->final_player->GetIndex() + 1) + " can steal 4 points from another team";
	this->description_texture = LoadText(description.c_str(), this->ren, Global::EVENT::STORY_FONT_SIZE, "font", 255, 255, 255);
	
	this->input = new Input();
	for (int i = 0; i <= 6; i++) {
		string str = "Select your target : Team " + (i ? std::to_string(i) : "?");
		this->input->AddString(str, this->ren);
	}
	return 0;
}

int FinalHumanScene::EventProcess(Event& evt) {
	if (this->is_activated_get_input && this->start_transition_level == this->transition_level_delta) {
		int input_index = this->input->ProcessEvent(evt);
		if (input_index) {
			if (input_index - 1 != this->game->final_player->GetIndex()) {
				this->game->score[this->game->final_player->GetIndex()] += Global::GAME::FINAL_HUMAN_MOVING_SCORE;
				this->game->score[input_index - 1] -= Global::GAME::FINAL_HUMAN_MOVING_SCORE;
				this->is_activated_get_input = false;
			}
			else {
				this->input->SetIndex(0);
			}
		}
		
	}
	return 0;
}

int FinalHumanScene::NormalProcess() {
	return 0;
}
