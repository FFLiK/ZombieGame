#pragma once
#include <string>
#include <atomic>
namespace Global {
	class WIN {
	public:
		static const int SCREEN_WIDTH = 1280;
		static const int SCREEN_HEIGHT = 720;

		static const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;
		static const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;

		static const int FPS = 120;

		static constexpr double SIZE_MULTIPLIER = SCREEN_WIDTH / 1280.0;
		static constexpr double FRAME_RATE_MULTIPLIER = FPS / 60.0;
	};

	class GAME {
	public:
		static constexpr double HEXAGON_SIZE = 35;
		static constexpr double PLAYER_SIZE = 20;
		static constexpr double SUPER_ZOMBIE_SIZE = 25;

		static constexpr int TIME_LIMIT = 30;

		static constexpr int PLAYER_FONT_SIZE = 24;
		static constexpr int SCORE_FONT_SIZE = 24;
		static constexpr int TIMER_FONT_SIZE = 96;

		static const int HUMAN_MOVING_SCORE = 1;
		static const int FINAL_HUMAN_MOVING_SCORE = 8;
		static const int INFECTED_SUPER_ZOMBIE_PENALTY_SCORE = -3;
	};

	class SYSTEM {
	public:
		static constexpr const char* NAME = "The Last Human";
		static constexpr const char* DEV_NAME = "Zombie Game";
		static constexpr const char* VERSION = "1.1";
		static const bool IS_RELEASE = false;
		static constexpr const char* DEVELOPER = "FFLiK";

		static const bool USE_APPDATA = true;
		static constexpr const char* RESOURCE_PATH = "res\\";
		static constexpr const char* IMAGE_EXTENSION = ".asset"; // .png or .asset
		static constexpr const char* FONT_EXTENSION = ".type"; // .ttf or .type

		static const bool DEBUG_MODE = false;
		static const bool TEXTURE_RENDERING = false;

		static int const TITLE_FONT_SIZE = 60;
		static int const DEV_FONT_SIZE = 16;
		static int const VER_FONT_SIZE = 24;
	};

	class EVENT {
	public:
		static int const MAIN_FONT_SIZE = 48;
	};
}

