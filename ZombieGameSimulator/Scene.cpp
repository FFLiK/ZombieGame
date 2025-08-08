#include "Scene.h"
#include <iostream>
#include <Log.h>
using namespace std;

Scene::Scene() : ren(nullptr) {  
   Log::FormattedDebug("Scene", "Constructor", "Calling constructor of Scene");  
}

Scene::~Scene() {
	Log::FormattedDebug("Scene", "Destructor", "Calling destructor of Scene");
}

int Scene::RegisterRenderer(SDL_Renderer* ren) {
    this->ren = ren;
    return 0;
}

int Scene::PushEvent(EventType T, SDL_Keycode key) {
    this->event_queue.push({ T, key, 0, 0, 0, 0, MOUSE_NONE });
    return 0;
}

int Scene::PushEvent(EventType T, int x, int y, int x_rel, int y_rel, EventMouse mt) {
	this->event_queue.push({ T, 0, x, y, x_rel, y_rel, mt });
	return 0;
}

int Scene::__Process__() {
    while (!this->event_queue.empty()) {
        this->EventProcess(this->event_queue.front());
        this->event_queue.pop();
    }
    this->NormalProcess();
    return 0;
}