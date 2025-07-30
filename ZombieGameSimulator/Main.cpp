#include <SDL.h>
#include <iostream>
#include <Window.h>
#include <Scene.h>
#include <Log.h>
#include <BackgroundScene.h>
#include <GameScene.h>
using namespace std;

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	Window* win = new Window({"Zombie Game", 1280, 720, 60});
	win->Execute();

	Game game;

	Scene* scene;
	scene = new GameScene(win, &game);
	win->AddScene(scene, 0);
	scene = new BackgroundScene();
	win->AddScene(scene, 1);

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
	SDL_Quit();
	return 0;
}