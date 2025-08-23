#include "EventScene.h"
#include "Log.h"
#include "Texture.h"
#include "Resources.h"
#include "Global.h"
#include "Hexagon.h"
#include <random>

EventScene::EventScene(Game* game) {
	this->game = game;
	Log::FormattedDebug("EventScene", "Constructor", "Calling constructor of EventScene");

	std::mt19937 rng(std::random_device{}());

	std::uniform_int_distribution<int> dist(0, 8);
	this->game_event = static_cast<GameEvent>(dist(rng));
	this->event_stroy_seed = std::uniform_int_distribution<int>(0, INT32_MAX)(rng);

	this->background_alpha = 0;
	this->current_text = nullptr;
	this->text_alpha = 0;
	this->level = 0;
	
	this->is_finished = false;

	this->execute_event_level = 0;
	this->slides.clear();

	this->enable_event_process = false;
	this->target_hexagon = nullptr;
	this->pause = false;

	this->input = nullptr;
}

EventScene::~EventScene() {
	Log::FormattedDebug("EventScene", "Destructor", "Calling destructor of EventScene");

	for (auto& slide : slides) {
		if (slide.texture != nullptr && slide.appearance == Slide::APPEAR) {
			SDL_DestroyTexture(slide.texture);
			slide.texture = nullptr;
		}
	}

	for (auto& board : roulette_board_num) {
		if (board.texture != nullptr) {
			SDL_DestroyTexture(board.texture);
			board.texture = nullptr;
		}
	}


}

int EventScene::Rendering() {
	for (auto& slide : slides) {
		if (this->level >= slide.level && this->level <= slide.level + transition_level_delta) {
			if (slide.type == Slide::BACKGROUND) {
				this->current_background = slide.texture;
				if (slide.appearance == Slide::APPEAR) {
					this->background_alpha = 255 * (this->level - slide.level) / transition_level_delta;
				}
				else if (slide.appearance == Slide::SEMI_APPEAR) {
					this->background_alpha = 255 - (55 * (this->level - slide.level) / transition_level_delta);
				}
				else {
					this->background_alpha = 200 - (200 * (this->level - slide.level) / transition_level_delta);
				}
			}
			else if (slide.type == Slide::TEXT) {
				this->current_text = slide.texture;
				if (slide.appearance == Slide::APPEAR) {
					this->text_alpha = 255 * (this->level - slide.level) / transition_level_delta;
				}
				else {
					this->text_alpha = 255 - (255 * (this->level - slide.level) / transition_level_delta);
				}
			}
			else if (slide.type == Slide::ROULETTE) {
				if (slide.appearance == Slide::APPEAR) {
					this->roulette_alpha = 255 * (this->level - slide.level) / transition_level_delta;
				}
				else {
					this->roulette_alpha = 255 - (255 * (this->level - slide.level) / transition_level_delta);
				}

			}
			else {
				if (slide.appearance == Slide::APPEAR) {
					if (!this->input) {
						if (this->game_event == GameEvent::SWAP_POSITION_WITH_TEAM) {
							this->input = new Input();
							for (int i = 0; i < this->game->GetPlayers()->size(); ++i) {
								string str = "Swap with team " + (i ? std::to_string(i) : "?");
								this->input->AddString(str, this->ren);
							}
						}
						else if (this->game_event == GameEvent::CHANGE_TEAM_STATE) {
							this->input = new Input();
							for (int i = 0; i < this->game->GetPlayers()->size(); ++i) {
								string str = "Change state of team " + (i ? std::to_string(i) : "?");
								this->input->AddString(str, this->ren);
							}
						}
					}
				}
				else {
					if (this->input) {
						delete this->input;
						this->input = nullptr;
					}
				}
			}
		}
	}

	SDL_Rect background;
	background.x = 0;
	background.y = 0;
	background.w = Global::WIN::SCREEN_WIDTH;
	background.h = Global::WIN::SCREEN_HEIGHT;


	if (this->background_alpha > 0) {
		if (this->current_background) {
			SDL_SetTextureBlendMode(this->current_background, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(this->current_background, this->background_alpha);
			SDL_RenderCopy(this->ren, this->current_background, NULL, &background);
			SDL_SetTextureAlphaMod(this->current_background, 255);
		}
		else {
			SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(this->ren, 0, 0, 0, this->background_alpha);
			SDL_RenderFillRect(this->ren, &background);
		}
	}

	if (this->current_text != nullptr && this->text_alpha > 0) {
		bool use_input = this->input != nullptr;

		SDL_Rect src, dst;
		SDL_QueryTexture(this->current_text, NULL, NULL, &src.w, &src.h);
		src.x = 0;
		src.y = 0;
		dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
		dst.y = (Global::WIN::SCREEN_HEIGHT - src.h) / 2;
		if (use_input) {
			dst.y = Global::WIN::SCREEN_HEIGHT_HALF - 20 * Global::WIN::SIZE_MULTIPLIER - src.h;
		}
		dst.w = src.w;
		dst.h = src.h;
		SDL_SetTextureAlphaMod(this->current_text, this->text_alpha);
		SDL_RenderCopy(this->ren, this->current_text, &src, &dst);
		SDL_SetTextureAlphaMod(this->current_text, 255);

		if (use_input) {
			SDL_Texture* input_tex = this->input->GetTexture();
			SDL_SetTextureBlendMode(input_tex, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(input_tex, this->text_alpha);
			SDL_QueryTexture(input_tex, NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.x = (Global::WIN::SCREEN_WIDTH - src.w) / 2;
			dst.y = Global::WIN::SCREEN_HEIGHT_HALF + 20 * Global::WIN::SIZE_MULTIPLIER;
			dst.w = src.w;
			dst.h = src.h;
			SDL_RenderCopy(this->ren, input_tex, &src, &dst);
		}
	}

	if (this->roulette_alpha > 0) {
		if (Global::SYSTEM::TEXTURE_RENDERING && Resources::event_roulette_num && Resources::event_roulette_sign) {
			SDL_Point center;
			center.x = Global::WIN::SCREEN_WIDTH / 4;
			center.y = Global::WIN::SCREEN_HEIGHT / 2;
			SDL_SetTextureBlendMode(Resources::event_roulette_num, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(Resources::event_roulette_num, this->roulette_alpha);
			SDL_Rect src, dst;
			SDL_QueryTexture(Resources::event_roulette_num, NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.w = src.w * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			dst.h = src.h * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			dst.x = center.x - dst.w / 2;
			dst.y = center.y - dst.h / 2;
			SDL_RenderCopyEx(this->ren, Resources::event_roulette_num, &src, &dst, this->roulette_angle_num, NULL, SDL_FLIP_NONE);
		
			SDL_SetTextureBlendMode(Resources::roulette_pin, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(Resources::roulette_pin, this->roulette_alpha);
			SDL_Rect pin_src, pin_dst;
			SDL_QueryTexture(Resources::roulette_pin, NULL, NULL, &pin_src.w, &pin_src.h);
			pin_src.x = 0;
			pin_src.y = 0;
			pin_dst.w = pin_src.w * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			pin_dst.h = pin_src.h * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			pin_dst.x = center.x - pin_dst.w / 2;
			pin_dst.y = center.y - pin_dst.h / 2 - dst.h / 2 * 0.9;
			SDL_RenderCopy(this->ren, Resources::roulette_pin, &pin_src, &pin_dst);

			center.x = Global::WIN::SCREEN_WIDTH * 3 / 4;
			SDL_SetTextureBlendMode(Resources::event_roulette_sign, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(Resources::event_roulette_sign, this->roulette_alpha);
			SDL_QueryTexture(Resources::event_roulette_sign, NULL, NULL, &src.w, &src.h);
			src.x = 0;
			src.y = 0;
			dst.w = src.w * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			dst.h = src.h * Global::EVENT::ROULETTE_SIZE / Global::WIN::SCREEN_HEIGHT * 0.8;
			dst.x = center.x - dst.w / 2;
			dst.y = center.y - dst.h / 2;
			SDL_RenderCopyEx(this->ren, Resources::event_roulette_sign, &src, &dst, this->roulette_angle_sign, NULL, SDL_FLIP_NONE);
		
			pin_dst.x = center.x - pin_dst.w / 2;
			pin_dst.y = center.y - pin_dst.h / 2 - dst.h / 2 * 0.9;
			SDL_RenderCopy(this->ren, Resources::roulette_pin, &pin_src, &pin_dst);
		}
		else {
			SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
			int rad = Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER;
			SDL_Point center;
			center.x = Global::WIN::SCREEN_WIDTH / 4;
			center.y = Global::WIN::SCREEN_HEIGHT / 2;
			filledCircleRGBA(this->ren, center.x, center.y, rad, 0, 0, 0, this->roulette_alpha);
			circleRGBA(this->ren, center.x, center.y, rad, 255, 255, 255, this->roulette_alpha);
			int angle = this->roulette_angle_num;
			for (RouletteBoard& board : this->roulette_board_num) {
				int start_angle = angle;
				int end_angle = angle + static_cast<int>(board.fraction * 360);
				SDL_Point start, end;
				start.x = center.x + rad * sin(start_angle * M_PI / 180.0);
				start.y = center.y + rad * -cos(start_angle * M_PI / 180.0);
				end.x = center.x + rad * sin(end_angle * M_PI / 180.0);
				end.y = center.y + rad * -cos(end_angle * M_PI / 180.0);
				SDL_SetRenderDrawColor(this->ren, 255, 255, 255, this->roulette_alpha);
				SDL_RenderDrawLine(this->ren, center.x, center.y, start.x, start.y);
				SDL_RenderDrawLine(this->ren, center.x, center.y, end.x, end.y);
				SDL_Point text_pos;
				text_pos.x = center.x + (rad * 0.5) * sin((start_angle + end_angle) * M_PI / 360.0);
				text_pos.y = center.y + (rad * 0.5) * -cos((start_angle + end_angle) * M_PI / 360.0);
				if (board.texture) {
					SDL_SetTextureBlendMode(board.texture, SDL_BLENDMODE_BLEND);
					SDL_SetTextureAlphaMod(board.texture, this->roulette_alpha);
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
			SDL_SetRenderDrawColor(this->ren, 255, 0, 0, this->roulette_alpha);
			SDL_RenderDrawLine(this->ren, center.x, center.y - rad, center.x, center.y - rad + 30 *Global::WIN::SIZE_MULTIPLIER);

			center.x = Global::WIN::SCREEN_WIDTH * 3 / 4;
			filledCircleRGBA(this->ren, center.x, center.y, rad, 0, 0, 0, this->roulette_alpha);
			circleRGBA(this->ren, center.x, center.y, rad, 255, 255, 255, this->roulette_alpha);
			angle = this->roulette_angle_sign;
			for (RouletteBoard& board : this->roulette_board_sign) {
				int start_angle = angle;
				int end_angle = angle + static_cast<int>(board.fraction * 360);
				SDL_Point start, end;
				start.x = center.x + rad * sin(start_angle * M_PI / 180.0);
				start.y = center.y + rad * -cos(start_angle * M_PI / 180.0);
				end.x = center.x + rad * sin(end_angle * M_PI / 180.0);
				end.y = center.y + rad * -cos(end_angle * M_PI / 180.0);
				SDL_SetRenderDrawColor(this->ren, 255, 255, 255, this->roulette_alpha);
				SDL_RenderDrawLine(this->ren, center.x, center.y, start.x, start.y);
				SDL_RenderDrawLine(this->ren, center.x, center.y, end.x, end.y);
				SDL_Point text_pos;
				text_pos.x = center.x + (rad * 0.5) * sin((start_angle + end_angle) * M_PI / 360.0);
				text_pos.y = center.y + (rad * 0.5) * -cos((start_angle + end_angle) * M_PI / 360.0);
				if (board.texture) {
					SDL_SetTextureBlendMode(board.texture, SDL_BLENDMODE_BLEND);
					SDL_SetTextureAlphaMod(board.texture, this->roulette_alpha);
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

				// draw pin
				SDL_SetRenderDrawColor(this->ren, 255, 0, 0, this->roulette_alpha);
				SDL_RenderDrawLine(this->ren, center.x, center.y - rad, center.x, center.y - rad + 30 * Global::WIN::SIZE_MULTIPLIER);
			}
		}
	}

	if (this->level == this->execute_event_level) {
		switch (this->game_event) {
		case GameEvent::INSTANT_EXTRA_MOVE: {
			this->game->current_turn--;
			if (this->game->current_turn < 0) {
				this->game->current_turn = this->game->GetPlayers()->size() - 1;
			}
		} break;
		case GameEvent::SWAP_POSITION_WITH_TEAM: {
			if (!this->enable_event_process && this->pause) {
				this->pause = false;
				this->enable_event_process = false;
			}
			else {
				this->pause = true;
				this->enable_event_process = true;
			}
		} break;
		case GameEvent::MOVE_SUPER_ZOMBIE_TO_TILE: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				if (target_hexagon->GetProperty() == HEXAGON_PAPAL) {
					Log::System("EventScene", "Rendering", "Super zombie cannot move to papal hexagon (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
					this->target_hexagon = nullptr;
				}
				else {
					this->pause = false;
					vector<Hexagon*> path;
					path.push_back(this->target_hexagon);
					path.push_back(this->game->GetHexagon(this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetX(), this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetY()));
					this->game->players[this->game->SUPER_ZOMBIE_INDEX].SetPosition(this->target_hexagon->GetX(), this->target_hexagon->GetY(), &path);
					Log::System("EventScene", "Rendering", "Super zombie moved to tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
				}
			}

		} break;
		case GameEvent::MOVE_SUPER_ZOMBIE_NEARBY: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				if (target_hexagon->GetProperty() == HEXAGON_PAPAL) {
					Log::System("EventScene", "Rendering", "Super zombie cannot move to papal hexagon (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
					this->target_hexagon = nullptr;
				}
				else if (abs(this->game->event_triggered_player->GetX() - this->target_hexagon->GetX()) > 1.0
					|| abs(this->game->event_triggered_player->GetY() - this->target_hexagon->GetY()) > 0.5) {
					Log::System("EventScene", "Rendering", "Super zombie cannot move to tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ") because it is not adjacent");
					this->target_hexagon = nullptr;
				}
				else {
					this->pause = false;
					vector<Hexagon*> path;
					path.push_back(this->target_hexagon);
					path.push_back(this->game->GetHexagon(this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetX(), this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetY()));
					this->game->players[this->game->SUPER_ZOMBIE_INDEX].SetPosition(this->target_hexagon->GetX(), this->target_hexagon->GetY(), &path);
					Log::System("EventScene", "Rendering", "Super zombie moved to tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
				}
			}
		} break;
		case GameEvent::CHANGE_TEAM_STATE: {
			if (!this->enable_event_process && this->pause) {
				this->pause = false;
				this->enable_event_process = false;
			}
			else {
				this->pause = true;
				this->enable_event_process = true;
			}
		} break;
		case GameEvent::CHANGE_OWN_STATE: {
			PlayerState new_state = (this->game->GetCurrentPlayer()->GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
			this->game->GetCurrentPlayer()->SetState(new_state);
			Log::System("EventScene", "Rendering", "State of your team changed to " + (new_state == PLAYER_HUMAN) ? "HUMAN" : "ZOMBIE");
		} break;
		case GameEvent::REVERSE_ALL_STATES: {
			for (int i = 0; i < this->game->GetPlayers()->size() - 1; ++i) {
				if (this->game->GetHexagon(this->game->GetPlayers()->at(i).GetX(), this->game->GetPlayers()->at(i).GetY())->GetProperty() == HEXAGON_PAPAL) {
					continue;
				}
				if (this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetX() == this->game->players[i].GetX()
					&& this->game->players[this->game->SUPER_ZOMBIE_INDEX].GetY() == this->game->players[i].GetY()) {
					continue;
				}
				/*
				bool pass = false;
				for (int j = 0; j < this->game->GetPlayers()->size(); ++j) {
					if (i != j
						&& this->game->players[i].GetX() == this->game->players[j].GetX()
						&& this->game->players[i].GetY() == this->game->players[j].GetY()) {
						pass = true;
					}
				}
				if (pass) continue;
				*/
				PlayerState new_state = (this->game->GetPlayers()->at(i).GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
				this->game->GetPlayers()->at(i).SetState(new_state);
				Log::System("EventScene", "Rendering", "State of team " + std::to_string(i + 1) + " changed to " + ((new_state == PLAYER_HUMAN) ? "HUMAN" : "ZOMBIE"));
			}
		} break;
		case GameEvent::CREATE_OBSTACLE: {
			if (target_hexagon == nullptr) {
				this->pause = true;
				this->enable_event_process = true;
			}
			else {
				this->pause = false;
				this->target_hexagon->SetProperty(HEXAGON_OBSTACLE);
				Log::System("EventScene", "Rendering", "Obstacle created at tile (" + std::to_string(this->target_hexagon->GetX()) + ", " + std::to_string(this->target_hexagon->GetY()) + ")");
			}
		} break;
		case GameEvent::SCORE_ROULETTE_EVENT: {
			if (this->roulette_active_num == -1) {
				this->pause = true;
				this->roulette_active_num = Spin::NATURAL_NUMBER_ROULETTE_SPIN | Spin::SIGN_ROULETTE_SPIN;
			}
			else if (this->roulette_active_num == Spin::NO_SPIN) {
				this->pause = false;
				int score = 0;
				double num_roulette_fraction = (double)(360 - this->roulette_angle_num) / 360.0;
				double sign_roulette_fraction = (double)(360 - this->roulette_angle_sign) / 360.0;
				for (const auto& board : this->roulette_board_num) {
					if (num_roulette_fraction < board.fraction) {
						score = board.data;
						break;
					}
					num_roulette_fraction -= board.fraction;
				}
				for (const auto& board : this->roulette_board_sign) {
					if (sign_roulette_fraction < board.fraction) {
						score *= board.data;
						break;
					}
					sign_roulette_fraction -= board.fraction;
				}
				SDL_Texture* score_event_final_texture = LoadText(("Team " + std::to_string(this->game->GetCurrentTurn() + 1) + " wins " + (score > 0 ? "+" : "") + std::to_string(score) + " points from punch machine.").c_str(), this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);

				this->slides.push_back(Slide(Slide::TEXT, 750 * Global::WIN::FRAME_RATE_MULTIPLIER, score_event_final_texture, Slide::APPEAR));
				this->slides.push_back(Slide(Slide::TEXT, 850 * Global::WIN::FRAME_RATE_MULTIPLIER, score_event_final_texture, Slide::DISAPPEAR));

				this->game->score[this->game->GetCurrentTurn()] += score;
				Log::System("EventScene", "Rendering", "Score roulette event executed. Added " + std::to_string(score) + " points to player " + std::to_string(this->game->GetCurrentTurn() + 1));
			}
			else {
				if (this->roulette_active_num & Spin::NATURAL_NUMBER_ROULETTE_SPIN) {
					this->roulette_angle_num += this->roulette_spin_speed;
					if (this->roulette_angle_num >= 360) {
						this->roulette_angle_num -= 360;
					}
				}
				if (this->roulette_active_num & Spin::SIGN_ROULETTE_SPIN) {
					this->roulette_angle_sign += this->roulette_spin_speed;
					if (this->roulette_angle_sign >= 360) {
						this->roulette_angle_sign -= 360;
					}
				}
			}
		} break;
		}
	}

	if (!pause) this->level++;
	if (this->slides.back().level + this->transition_level_delta < this->level) {
		this->is_finished = true;
	}
	return 0;
}

bool EventScene::IsEnd() {
	return this->is_finished;
}

int EventScene::ProcessInit() {
	string event_main_text, story_text;
	SDL_Texture* event_background_tex = nullptr;

	switch (this->game_event) {
	case GameEvent::INSTANT_EXTRA_MOVE:
		event_main_text = "Move Again Immediately";
		if (this->event_stroy_seed % 2) {
			story_text = "\"It feels like... I should start running!\"";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(0);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		else {
			story_text = "A sudden explosion nearby\nmakes you instinctively throw yourself\ninto the floor again.";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(1);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}

		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::SWAP_POSITION_WITH_TEAM:
		event_main_text = "Swap Positions with Any Team";
		story_text = "\"I'm sorry but... I¡¯ve got no choice.\"";
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			Resources::event_pdf->SetPage(2);
			event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::MOVE_SUPER_ZOMBIE_TO_TILE:
		event_main_text = "Move The Super Zombie to Any Tile";
		if (this->event_stroy_seed % 2) {
			story_text = "\"Yes, let¡¯s make a loud noise over there.\"";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(3);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		else {
			story_text = "\"Grrhh..\"\nWait, did the super zombie just understand my words?\n\"Hey, there¡¯s something really tempting over there!!\"";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(4);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}

		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::MOVE_SUPER_ZOMBIE_NEARBY:
		event_main_text = "Move The Super Zombie\nRight Next to Your Team";
		story_text = "\"I haven¡¯t showered for 3 days...\nguess I smell a bit too much.\"";
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			Resources::event_pdf->SetPage(5);
			event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::CHANGE_TEAM_STATE:
		event_main_text = "Force One Team¡¯s State to Change";
		if (this->event_stroy_seed % 2) {
			story_text = "\"You can't fool my eyes.\"\nThe rain washes away the makeup,\nrevealing someone¡¯s true identity.";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(7);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		else {
			story_text = "A boiling liquid drops from the ceiling,\nburning your skin.";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(6);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::CHANGE_OWN_STATE:
		event_main_text = "Reverse Your Own State";
		if (this->event_stroy_seed % 2) {
			story_text = "There is a bruise on your shoulder...\nnow you finally understand\nwhat the burning feeling means.";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(8);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		else {
			story_text = "You found a suspicious drink lying on the street.\n\"Perfect timing! I was thirsty...\"\n\"Wait, what¡¯s happening?\"";
			if (Global::SYSTEM::TEXTURE_RENDERING) {
				Resources::event_pdf->SetPage(9);
				event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
			}
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::REVERSE_ALL_STATES:
		event_main_text = "Reverse The State of All Teams";
		story_text = "The sky turns crimson,\nand a great upheaval reverses the order of this world.";
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			Resources::event_pdf->SetPage(10);
			event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::CREATE_OBSTACLE:
		event_main_text = "Block A Tile";
		story_text = "The remaining ruins of a building collapsed,\nblocking the way.";
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			Resources::event_pdf->SetPage(11);
			event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
		}
		this->execute_event_level = 450 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;

	case GameEvent::SCORE_ROULETTE_EVENT:
		event_main_text = "Bonus point!";
		story_text = "You¡¯ve found a punching machine.\n\"Shall I test my power?\"";
		if (Global::SYSTEM::TEXTURE_RENDERING) {
			Resources::event_pdf->SetPage(12);
			event_background_tex = Resources::event_pdf->GetCurrentPageTexture();
		}
		this->execute_event_level = 600 * Global::WIN::FRAME_RATE_MULTIPLIER + transition_level_delta;
		break;
	}

	Log::FormattedDebug("EventScene", "ProcessInit", "Initializing EventScene with game event: " + event_main_text);
	Log::System("EventScene", "ProcessInit", "Game event: " + event_main_text);

	SDL_Texture* event_main_text_tex = LoadText(event_main_text.c_str(), this->ren, Global::EVENT::MAIN_FONT_SIZE, "font", 255, 255, 255);
	SDL_Texture* event_story_text_tex = LoadText(story_text.c_str(), this->ren, Global::EVENT::STORY_FONT_SIZE, "font", 255, 255, 255);

	this->slides = {
		Slide(Slide::BACKGROUND, 0, event_background_tex, Slide::APPEAR),
		Slide(Slide::TEXT, 50, event_story_text_tex, Slide::APPEAR),
		Slide(Slide::TEXT, 350, event_story_text_tex, Slide::DISAPPEAR),
		Slide(Slide::BACKGROUND, 400, event_background_tex, Slide::SEMI_APPEAR),
		Slide(Slide::TEXT, 450, event_main_text_tex, Slide::APPEAR),
		Slide(Slide::INPUT, 450, nullptr, Slide::APPEAR),
		Slide(Slide::TEXT, 550, event_main_text_tex, Slide::DISAPPEAR),
		Slide(Slide::INPUT, 550 + this->original_transition_level_delta, nullptr, Slide::DISAPPEAR),
		Slide(Slide::BACKGROUND, 550, event_background_tex, Slide::DISAPPEAR),
	};

	if (this->game_event == GameEvent::SCORE_ROULETTE_EVENT) {
		this->slides.pop_back();
		this->slides.push_back(Slide(Slide::ROULETTE, 600, nullptr, Slide::APPEAR));
		this->slides.push_back(Slide(Slide::ROULETTE, 700, nullptr, Slide::DISAPPEAR));
		this->slides.push_back(Slide(Slide::BACKGROUND, 850, nullptr, Slide::DISAPPEAR));
	}
	
	for (int i = 0; i < this->slides.size(); ++i) {
		this->slides[i].level *= Global::WIN::FRAME_RATE_MULTIPLIER;
	}

	this->roulette_board_num = {
		RouletteBoard(0.3, 1, LoadText("1", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.1, 5, LoadText("5", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.2, 2, LoadText("2", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.05, 7, LoadText("7", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.2, 1, LoadText("1", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.15, 2, LoadText("2", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
	};

	this->roulette_board_sign = {
		RouletteBoard(0.25, 1, LoadText("+", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.05, -1, LoadText("-", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.25, 1, LoadText("+", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.05, -1, LoadText("-", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.25, 1, LoadText("+", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
		RouletteBoard(0.15, -1, LoadText("-", this->ren, Global::EVENT::ROULETTE_FONT_SIZE, "font", 255,255,255)),
	};
	return 0;
}

int EventScene::EventProcess(Event& evt) {
	if (this->enable_event_process) {
		if (this->input) {
			int data = this->input->ProcessEvent(evt);
			if (data) {
				bool pass = true;
				if (this->game_event == GameEvent::SWAP_POSITION_WITH_TEAM) {
					int target = data - 1;
					if (this->game->current_turn == target) {
						pass = false;
					}
					else {
						double x1 = this->game->players[this->game->current_turn].GetX();
						double y1 = this->game->players[this->game->current_turn].GetY();
						double x2 = this->game->players[target].GetX();
						double y2 = this->game->players[target].GetY();
						vector<Hexagon*> path1, path2;
						path1.push_back(game->GetHexagon(x2, y2));
						path1.push_back(game->GetHexagon(x1, y1));
						path2.push_back(game->GetHexagon(x1, y1));
						path2.push_back(game->GetHexagon(x2, y2));
						this->game->players[this->game->current_turn].SetPosition(x2, y2, &path1);
						this->game->players[target].SetPosition(x1, y1, &path2);

						if (this->game->GetHexagon(x2, y2)->GetProperty() == HEXAGON_PAPAL) {
							this->game->players[this->game->current_turn].SetState(PLAYER_HUMAN);
						}

						for (int i = 0; i < this->game->GetPlayers()->size(); ++i) {
							if (i != target && i != this->game->current_turn) {
								if (x2 == this->game->players[i].GetX() && y2 == this->game->players[i].GetY()) {
									this->game->players[this->game->current_turn].SetState(PLAYER_ZOMBIE);
								}
							}
						}
					}
				}
				else if (this->game_event == GameEvent::CHANGE_TEAM_STATE) {
					int target = data - 1;
					if (this->game->GetHexagon(this->game->players[target].GetX(), this->game->players[target].GetY())->GetProperty() == HEXAGON_PAPAL) {
						Log::System("EventScene", "Rendering", "Cannot change state of papal team " + std::to_string(target + 1));
						pass = false;
					}
					for (int i = 0; i < this->game->GetPlayers()->size(); ++i) {
						if (i != target
							&& this->game->players[target].GetX() == this->game->players[i].GetX()
							&& this->game->players[target].GetY() == this->game->players[i].GetY()) {
							if (this->game->players[i].GetState() == PLAYER_ZOMBIE) {
								Log::System("EventScene", "Rendering", "Cannot change state of team " + std::to_string(target + 1) + " because it is on the same tile as zombie team " + std::to_string(i + 1));
							}
							else {
								Log::System("EventScene", "Rendering", "Cannot change state of team " + std::to_string(target + 1) + " because it is on the same tile as super zombie");
							}
							pass = false;
						}
					}
					if (pass) {
						PlayerState new_state = (this->game->players[target].GetState() == PLAYER_HUMAN) ? PLAYER_ZOMBIE : PLAYER_HUMAN;
						this->game->players[target].SetState(new_state);
						Log::System("EventScene", "Rendering", "State of team " + std::to_string(target + 1) + " changed to " + ((new_state == PLAYER_HUMAN) ? "HUMAN" : "ZOMBIE"));
					}
				}

				if (pass) {
					this->enable_event_process = false;
				}
				else {
					this->input->SetIndex(0);
				}
			}
		}
		else {
			double mouse_x = evt.x;
			double mouse_y = evt.y;

			Player* player = game->GetCurrentPlayer();

			for (auto& hexagon : *(game->GetHexagons())) {
				bool flag = false;
				if (hexagon.IsInside(mouse_x, mouse_y)) {
					if (game->IsMovable(&hexagon, player, nullptr)) {
						flag = true;
					}
				}
				hexagon.SetActivated(flag);
				if (flag) {
					if (evt.mouse == MOUSE_LEFT && evt.T == MOUSE_UP) {
						target_hexagon = &hexagon;
					}
				}
			}
		}
	}

	if (this->roulette_active_num != NO_SPIN && this->roulette_active_num != -1) {
		if (evt.mouse == MOUSE_LEFT && evt.T == MOUSE_UP) {
			SDL_Point roulette_num_pos = { Global::WIN::SCREEN_WIDTH / 4, Global::WIN::SCREEN_HEIGHT / 2 };
			SDL_Point roulette_sign_pos = { Global::WIN::SCREEN_WIDTH * 3 / 4, Global::WIN::SCREEN_HEIGHT / 2 };
			int distance_num = sqrt(pow(evt.x - roulette_num_pos.x, 2) + pow(evt.y - roulette_num_pos.y, 2));
			int distance_sign = sqrt(pow(evt.x - roulette_sign_pos.x, 2) + pow(evt.y - roulette_sign_pos.y, 2));

			if (distance_num < Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER
				&& (this->roulette_active_num & Spin::NATURAL_NUMBER_ROULETTE_SPIN)) {
				this->roulette_active_num &= ~Spin::NATURAL_NUMBER_ROULETTE_SPIN;
			}
			else if (distance_sign < Global::EVENT::ROULETTE_SIZE * Global::WIN::SIZE_MULTIPLIER
				&& (this->roulette_active_num & Spin::SIGN_ROULETTE_SPIN)) {
				this->roulette_active_num &= ~Spin::SIGN_ROULETTE_SPIN;
			}
		}
	}

	if (evt.mouse == MOUSE_RIGHT && evt.T == MOUSE_UP) {
		pause = false;
		enable_event_process = false;
		level++;
		Log::System("EventScene", "EventProcess", "Event process cancelled by right-click.");
	}
	return 0;
}

int EventScene::NormalProcess() {
	return 0;
}
