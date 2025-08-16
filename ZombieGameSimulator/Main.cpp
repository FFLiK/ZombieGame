#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <Window.h>
#include <Scene.h>
#include <Log.h>
#include <BackgroundScene.h>
#include <GameScene.h>
#include <EventScene.h>
#include <Game.h>
#include <Global.h>
#include <Texture.h>
#include <Resources.h>
#include <PdfViewer.h>
using namespace std;

int main(int argc, char *argv[]) {
	Log::EnableThreadId(true);
	Log::EnableTimestamp(true);
	Log::SetPauseOnError(true);

	Log::OpenLogFile();

	Log::System("Zombie Game Simulator executed.");
	Log::System("SYSTEM INFO");
	Log::System("Project:", Global::SYSTEM::NAME, "(", Global::SYSTEM::DEV_NAME, ")");
	Log::System("Version:", Global::SYSTEM::VERSION);
	Log::System("Release:", Global::SYSTEM::IS_RELEASE ? "True" : "False");
	Log::System("Debug Mode:", Global::SYSTEM::DEBUG_MODE ? "True" : "False");

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	FPDF_InitLibrary();

	Log::System("Zombie Game Simulator started.");

	Window* win = new Window({Global::SYSTEM::NAME, Global::WIN::SCREEN_WIDTH, Global::WIN::SCREEN_HEIGHT, Global::WIN::FPS});
	win->Execute();

	Game game(win);

	Scene* scene;
	scene = new GameScene(&game);
	win->AddScene(scene, 0);
	scene = new BackgroundScene();
	win->AddScene(scene, 1);

	double next_save_time = win->RunTime();

	bool run = true;
	while (run) {
		switch (win->PollEvent()) {
		case QUIT:
			run = false;
			break;
		}

		if (next_save_time < win->RunTime()) {
			next_save_time = win->RunTime() + 1.0;
			Log::SaveLogFile();
		}
	}
	win->Destroy();
	delete win;

	Log::System("Zombie Game Simulator exited.");
	FPDF_DestroyLibrary();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	Log::CloseLogFile();
	return 0;
}