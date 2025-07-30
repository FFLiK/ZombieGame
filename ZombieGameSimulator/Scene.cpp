#include "Scene.h"
#include <thread>
#include <iostream>
#include <Log.h>
using namespace std;

Scene::Scene(){
    Log::FormattedDebug("Scene", "Constructor", "생성자 호출");
    this->run = true;
    this->process_completed = false;
    thread th([&]() {this->__Process__();});
    th.detach();
}

Scene::~Scene() {
    Log::FormattedDebug("Scene", "Destructor", "소멸자 호출");
    //TODO : Destroy Data
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
    while (this->run) {
        while (!this->event_queue.empty()) {
            this->EventProcess(this->event_queue.front());
            this->event_queue.pop();
        }
        this->NormalProcess();
    }
    this->process_completed = true;
    return 0;
}

int Scene::Destroy() {
	this->run = false;
	while (!this->process_completed);
	return 0;
}

bool Scene::IsRun() {
    return this->run;
}
