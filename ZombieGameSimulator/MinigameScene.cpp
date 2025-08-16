#include "MinigameScene.h"
#include "Resources.h"
#include "Texture.h"
#include "Log.h"
#include "Global.h"
#include <vector>
#include <string>
using namespace std;

MinigameScene::MinigameScene(Game* game) {
	this->game = game;
	this->is_finished = false;
	this->roulette_angle = 0;
	this->input = nullptr;
	this->is_activated_get_input = false;
}

MinigameScene::~MinigameScene() {
	for (auto& board : this->roulette_board) {
		if (board.texture) {
			SDL_DestroyTexture(board.texture);
			board.texture = nullptr;
		}
	}
	if (this->pdf_viewer) {
		this->pdf_viewer = nullptr;
	}
	if (this->title_texture) {
		SDL_DestroyTexture(this->title_texture);
		this->title_texture = nullptr;
	}
	if (this->pdf_texture) {
		SDL_DestroyTexture(this->pdf_texture);
		this->pdf_texture = nullptr;
	}
	if (this->input) {
		delete this->input;
		this->input = nullptr;
	}
}

int MinigameScene::Rendering() {

	double fraction;
	
	fraction = (double)this->start_transition_level / (double)this->transition_level_delta;
	if (this->end_transition_level) {
		fraction = (double)this->end_transition_level / (double)this->transition_level_delta;
		fraction = 1.0 - fraction;
	}

	int background_alpha = 255 * fraction;
	if (Global::SYSTEM::TEXTURE_RENDERING && Resources::minigame_background) {
		SDL_SetTextureBlendMode(Resources::minigame_background, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(Resources::minigame_background, background_alpha);
		SDL_RenderCopy(this->ren, Resources::minigame_background, nullptr, nullptr);
	}
	else {
		SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(this->ren, 0, 0, 0, background_alpha);
		SDL_RenderFillRect(this->ren, nullptr);
	}


	double f = (1.0 - (1.0 - fraction) * (1.0 - fraction));
	int roulette_position = (double)(-Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER) * (1.0 - f) + (double)(Global::WIN::SCREEN_HEIGHT_HALF) * f;

	if (f != 0) {
		if (Global::SYSTEM::TEXTURE_RENDERING && Resources::minigame_roulette) {

		}
		else {
			SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
			int rad = Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER;
			SDL_Point center;
			center.x = Global::WIN::SCREEN_WIDTH / 2;
			center.y = roulette_position;
			filledCircleRGBA(this->ren, center.x, center.y, rad, 0, 0, 0, 255);
			circleRGBA(this->ren, center.x, center.y, rad, 255, 255, 255, 255);
			int angle = this->roulette_angle;
			for (RouletteBoard board : this->roulette_board) {
				int start_angle = angle;
				int end_angle = angle + static_cast<int>(board.fraction * 360);
				SDL_Point start, end;
				start.x = center.x + rad * sin(start_angle * M_PI / 180.0);
				start.y = center.y + rad * -cos(start_angle * M_PI / 180.0);
				end.x = center.x + rad * sin(end_angle * M_PI / 180.0);
				end.y = center.y + rad * -cos(end_angle * M_PI / 180.0);
				SDL_SetRenderDrawColor(this->ren, 255, 255, 255, 255);
				SDL_RenderDrawLine(this->ren, center.x, center.y, start.x, start.y);
				SDL_RenderDrawLine(this->ren, center.x, center.y, end.x, end.y);
				SDL_Point text_pos;
				text_pos.x = center.x + (rad * 0.5) * sin((start_angle + end_angle) * M_PI / 360.0);
				text_pos.y = center.y + (rad * 0.5) * -cos((start_angle + end_angle) * M_PI / 360.0);
				if (board.texture) {
					SDL_Rect src, dst;
					SDL_QueryTexture(board.texture, NULL, NULL, &src.w, &src.h);
					src.x = 0;
					src.y = 0;
					dst.x = text_pos.x - src.w / 2;
					dst.y = text_pos.y - src.h / 2;
					dst.w = src.w;
					dst.h = src.h;
					SDL_Point text_center;
					text_center.x = dst.w / 2;
					text_center.y = dst.h / 2;
					SDL_RendererFlip flip = SDL_FLIP_NONE;
					SDL_RenderCopyEx(this->ren, board.texture, &src, &dst, (start_angle + end_angle) / 2, &text_center, SDL_FLIP_NONE);
				}
				angle = end_angle;
			}
			// draw pin
			SDL_SetRenderDrawColor(this->ren, 255, 0, 0, 255);
			SDL_RenderDrawLine(this->ren, center.x, center.y - rad, center.x, center.y - rad + 30 * Global::WIN::SIZE_MULTIPLIER);
		}
	}

	if (this->title_texture) {
		fraction = (double)this->title_transition_level / (double)this->transition_level_delta;
		if (this->end_transition_level) {
			fraction = (double)this->end_transition_level / (double)this->transition_level_delta;
			fraction = 1.0 - fraction;
		}
		SDL_SetTextureBlendMode(this->title_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(this->title_texture, 255 * fraction);
		SDL_Rect src, dst;
		SDL_QueryTexture(this->title_texture, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
		dst.y = Global::WIN::SCREEN_HEIGHT_HALF - 20 * Global::WIN::SIZE_MULTIPLIER - src.h;
		dst.w = src.w;
		dst.h = src.h;
		SDL_RenderCopy(this->ren, this->title_texture, &src, &dst);

		SDL_Texture* input_tex = this->input->GetTexture();
		SDL_SetTextureBlendMode(input_tex, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(input_tex, 255 * fraction);
		SDL_QueryTexture(input_tex, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
		dst.y = Global::WIN::SCREEN_HEIGHT_HALF + 20 * Global::WIN::SIZE_MULTIPLIER;
		dst.w = src.w;
		dst.h = src.h;
		SDL_RenderCopy(this->ren, input_tex, &src, &dst);
	}

	if (this->pdf_texture) {
		fraction = (double)this->pdf_start_transition_level / (double)this->transition_level_delta;
		if (this->pdf_end_transition_level) {
			fraction = (double)this->pdf_end_transition_level / (double)this->transition_level_delta;
			fraction = 1.0 - fraction;
		}
		SDL_SetTextureBlendMode(this->pdf_texture, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(this->pdf_texture, 255 * fraction);
		SDL_Rect src, dst;
		SDL_QueryTexture(this->pdf_texture, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
		dst.y = (Global::WIN::SCREEN_HEIGHT - src.h) / 2;
		dst.w = src.w;
		dst.h = src.h;
		SDL_RenderCopy(this->ren, this->pdf_texture, &src, &dst);
	}

	if (this->start_transition_level == this->transition_level_delta && !this->title_texture) {
		if (this->roulette_active_num == -1) {
			this->roulette_active_num = 1;
		}
		else if (this->roulette_active_num == 0) {
			int data = 0;
			double roulette_fraction = (double)(360 - this->roulette_angle) / 360.0;
			for (const auto& board : this->roulette_board) {
				if (roulette_fraction < board.fraction) {
					data = board.data;
					switch (data) {
					case 1:
						this->title_texture = LoadText("Cham Cham Cham", this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
						this->is_activated_get_input = true;
						break;
					case 2:
						this->title_texture = LoadText("Toy Hammer Game", this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
						this->is_activated_get_input = true;
						break;
					case 3:
						this->title_texture = LoadText("Emoji Game", this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
						this->pdf_viewer = Resources::minigame_emoji_game_pdf;
						break;
					case 4:
						this->title_texture = LoadText("Unified Voice Game", this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
						this->pdf_viewer = Resources::minigame_unified_voice_game_pdf;
						break;
					}
					break;
				}
				roulette_fraction -= board.fraction;
			}
		}
		else {
			this->roulette_angle += this->roulette_spin_speed;
			if (this->roulette_angle >= 360) {
				this->roulette_angle -= 360;
			}
		}
	}

	if (this->start_transition_level < this->transition_level_delta) {
		this->start_transition_level++;
	}
	else if (this->roulette_active_num != 0) {

	}
	else if (this->title_transition_level < this->transition_level_delta) {
		this->title_transition_level++;
	}
	else if (this->pdf_viewer || this->pdf_texture) {
		if (this->pdf_start_transition_level < this->transition_level_delta) {
			this->pdf_start_transition_level++;
			if (!this->pdf_texture) {
				this->pdf_texture = this->pdf_viewer->GetCurrentPageTexture();
			}
		}
		else if (this->pdf_viewer) {

		}
		else if (this->pdf_end_transition_level < this->transition_level_delta) {
			this->is_activated_get_input = true;
			this->pdf_end_transition_level++;
		}
		else {
			if (this->pdf_texture) {
				SDL_DestroyTexture(this->pdf_texture);
				this->pdf_texture = nullptr;
			}
		}
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

bool MinigameScene::IsEnd() {
	return this->is_finished;
}

int MinigameScene::ProcessInit() {
	this->start_transition_level = 0;
	this->end_transition_level = 0;

	roulette_board.push_back(RouletteBoard(0.25, 1, LoadText("Cham\nCham\nCham", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255, 255, 255)));
	roulette_board.push_back(RouletteBoard(0.25, 2, LoadText("Toy\nHammer\nGame", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255, 255, 255)));
	roulette_board.push_back(RouletteBoard(0.25, 3, LoadText("Emoji\nGame", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255, 255, 255)));
	roulette_board.push_back(RouletteBoard(0.25, 4, LoadText("Unified\nVoice\nGame", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255, 255, 255)));
	
	this->input = new Input();
	for (int i = 0; i <= 4; i++) {
		string str = "Team " + std::to_string(this->game->minigame_triggered_player->GetIndex() + 1) + " gets " + (i ? "+" + to_string(i) : "?") + " points";
		this->input->AddString(str, this->ren);
	}
	return 0;
}

int MinigameScene::EventProcess(Event& evt) {
	if (this->start_transition_level == this->transition_level_delta) {
		if (evt.mouse == SDL_BUTTON_LEFT && evt.T == MOUSE_UP) {
			int distance = sqrt(pow(evt.x - Global::WIN::SCREEN_WIDTH / 2, 2) + pow(evt.y - Global::WIN::SCREEN_HEIGHT_HALF, 2));
			if (distance < Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER / 2) {
				this->roulette_active_num = 0;
			}
		}
	}
	if (this->pdf_viewer) {
		if (evt.T == KEY_DOWN && (evt.key == SDLK_RIGHT || evt.key == SDLK_SPACE)) {
			this->pdf_viewer->NextPage();
			if (this->pdf_texture) {
				SDL_DestroyTexture(this->pdf_texture);
			}
			this->pdf_texture = this->pdf_viewer->GetCurrentPageTexture();
		}
		else if (evt.T == KEY_DOWN && evt.key == SDLK_LEFT) {
			this->pdf_viewer->PreviousPage();
			if (this->pdf_texture) {
				SDL_DestroyTexture(this->pdf_texture);
			}
			this->pdf_texture = this->pdf_viewer->GetCurrentPageTexture();
		}
		else if (evt.T == KEY_UP && (evt.key == SDLK_ESCAPE || evt.key == SDLK_RETURN)) {
			this->pdf_viewer = nullptr;
		}
	}
	if (this->is_activated_get_input) {
		int input_index = this->input->ProcessEvent(evt);
		if (input_index) {
			this->game->score[this->game->minigame_triggered_player->GetIndex()] += input_index;
			this->is_activated_get_input = false;

			Log::System("Player", this->game->minigame_triggered_player->GetIndex() + 1, "gets", input_index, "points from minigame.");
		}
	}
	return 0;
}

int MinigameScene::NormalProcess() {
	return 0;
}
