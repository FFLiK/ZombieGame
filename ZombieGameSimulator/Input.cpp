#include "Input.h"
#include "Texture.h"
#include "Log.h"

double Input::input_duration = 0.0;

Input::Input() {
	this->index = 0;
	this->input_textures.clear();
}

Input::~Input() {
	for (auto& texture : this->input_textures) {
		if (texture) {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
	}
	this->input_textures.clear();
}

void Input::AddString(std::string text, SDL_Renderer* ren) {
	this->input_textures.push_back(LoadText(text.c_str(), ren, Global::EVENT::STORY_FONT_SIZE, "font", 255, 255, 255));
}

SDL_Texture* Input::GetTexture() {
	if (this->index < 0 || this->index >= this->input_textures.size()) {
		return nullptr;
	}
	return this->input_textures[this->index];
}

int Input::ProcessEvent(Event& evt) {
	if (evt.T == KEY_DOWN) {
		if (evt.key == SDLK_RETURN) {
			return this->index;
		}
		else if (evt.key >= SDLK_1 && evt.key <= SDLK_9) {
			int input_value = evt.key - SDLK_1 + 1;
			if (input_value < this->input_textures.size()) {
				this->index = input_value;
			}
		}
		else if (evt.key >= SDLK_KP_1 && evt.key <= SDLK_KP_9) {
			int input_value = evt.key - SDLK_KP_1 + 1;
			if (input_value < this->input_textures.size()) {
				this->index = input_value;
			}
		}
	}
	return 0;
}

void Input::SetIndex(int idx) {
	if (idx < 0 || idx >= this->input_textures.size()) {
		Log::Error("Input index out of range: " + std::to_string(idx));
		return;
	}
	this->index = idx;
}
