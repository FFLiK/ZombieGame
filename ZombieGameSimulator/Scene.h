#pragma once
#include <SDL.h>
#include <Event.h>
#include <queue>
using namespace std;

class Scene {
public:
	Scene();
	virtual ~Scene();

	virtual int ProcessInit() abstract;
	virtual int Rendering() abstract;
	virtual int EventProcess(Event& evt) abstract;
	virtual int NormalProcess() abstract;

	int RegisterRenderer(SDL_Renderer* ren);
	int PushEvent(EventType T, SDL_Keycode key);
	int PushEvent(EventType T, int x, int y, int x_rel, int y_rel, EventMouse mt);

	int __Process__();

protected:
	SDL_Renderer* ren = nullptr;
	queue<Event> event_queue;
};

