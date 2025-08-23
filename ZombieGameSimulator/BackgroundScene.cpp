#include "BackgroundScene.h"
#include "Resources.h"
#include <Log.h>
#include <Global.h>
#include <Texture.h>

BackgroundScene::BackgroundScene() {
	Log::FormattedDebug("BackgroundScene", "Constructor", "Calling constructor of BackgroundScene");
}

BackgroundScene::~BackgroundScene() {
	Log::FormattedDebug("BackgroundScene", "Destructor", "Calling destructor of BackgroundScene");
	if (this->title_tex != nullptr) {
		SDL_DestroyTexture(this->title_tex);
		this->title_tex = nullptr;
	}
	if (this->developer_tex != nullptr) {
		SDL_DestroyTexture(this->developer_tex);
		this->developer_tex = nullptr;
	}
	if (this->version_tex != nullptr) {
		SDL_DestroyTexture(this->version_tex);
		this->version_tex = nullptr;
	}
}

int BackgroundScene::Rendering() {
	if (Global::SYSTEM::TEXTURE_RENDERING) {
		SDL_RenderCopy(this->ren, Resources::background, NULL, NULL);
	}
	SDL_Rect src, dst;
	src.x = 0;
	src.y = 0;
	SDL_QueryTexture(this->title_tex, NULL, NULL, &src.w, &src.h);
	dst.w = src.w;
	dst.h = src.h;
	dst.x = 20 * Global::WIN::SIZE_MULTIPLIER;
	dst.y = 20 * Global::WIN::SIZE_MULTIPLIER;
	SDL_RenderCopy(this->ren, this->title_tex, &src, &dst);

	SDL_QueryTexture(this->developer_tex, NULL, NULL, &src.w, &src.h);
	dst.w = src.w;
	dst.h = src.h;
	dst.y = Global::WIN::SCREEN_HEIGHT - 10 * Global::WIN::SIZE_MULTIPLIER - src.h;
	SDL_RenderCopy(this->ren, this->developer_tex, &src, &dst);

	SDL_QueryTexture(this->version_tex, NULL, NULL, &src.w, &src.h);
	dst.w = src.w;
	dst.h = src.h;
	dst.x = Global::WIN::SCREEN_WIDTH - 20 * Global::WIN::SIZE_MULTIPLIER - src.w;
	dst.y = Global::WIN::SCREEN_HEIGHT - 10 * Global::WIN::SIZE_MULTIPLIER - src.h;
	SDL_RenderCopy(this->ren, this->version_tex, &src, &dst);
	return 0;
}


int BackgroundScene::ProcessInit() {
	string title_text = Global::SYSTEM::NAME;
	for (int i = 0; i < title_text.size(); i++) {
		if (title_text[i] == ' ') {
			title_text[i] = '\n';
		}
	}
	this->title_tex = LoadText(title_text.c_str(), this->ren, Global::SYSTEM::TITLE_FONT_SIZE, "font", 255, 255, 255, -1, -10);
	this->developer_tex = LoadText(((string)"Developed by " + Global::SYSTEM::DEVELOPER).c_str(), this->ren, Global::SYSTEM::DEV_FONT_SIZE, "font", 255, 255, 255);
	std::string version_text = "ver" + std::string(Global::SYSTEM::VERSION) + "-" + (Global::SYSTEM::IS_RELEASE ? "release" : "pre-release") + (Global::SYSTEM::DEBUG_MODE ? " w.Debug Mode" : "");
	this->version_tex = LoadText(version_text.c_str(), this->ren, Global::SYSTEM::VER_FONT_SIZE, "font", 255, 255, 255);
	return 0;
}

int BackgroundScene::EventProcess(Event& evt) {
	return 0;
}

int BackgroundScene::NormalProcess() {
	return 0;
}