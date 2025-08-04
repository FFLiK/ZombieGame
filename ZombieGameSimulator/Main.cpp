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
using namespace std;

int main(int argc, char *argv[]) {
	srand((unsigned)time(nullptr));

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	Log::System("Zombie Game Simulator started.");

	Window* win = new Window({"Zombie Game", Global::WIN::SCREEN_WIDTH, Global::WIN::SCREEN_HEIGHT, Global::WIN::FPS});
	win->Execute();

	Game game(win);

	Scene* scene;
	scene = new GameScene(&game);
	win->AddScene(scene, 0);
	scene = new BackgroundScene();
	win->AddScene(scene, 1);

	game.Start();

	bool run = true;
	while (run) {
		switch (win->PollEvent()) {
		case QUIT:
			run = false;
			break;
		}
	}
	win->Destroy();
	delete win;

	Log::System("Zombie Game Simulator exited.");
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	system("pause");
	return 0;
}