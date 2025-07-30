#include "Window.h"
#include <ctime>
#include <thread>
#include <Log.h>

Window::Window(WindowData w_dat) {
	Log::FormattedDebug("Window", "Constructor", "������ ȣ��");
	if (this->run) {
		return;
	}
	this->run = true;
	this->rendering_completed = false;
	this->win = SDL_CreateWindow(w_dat.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_dat.width, w_dat.height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	this->ren = SDL_CreateRenderer(this->win, -1, SDL_RENDERER_ACCELERATED);
	this->fps = w_dat.fps;
	this->windows_created_time = clock();
}

Window::~Window() {
	if (this->ren)
		SDL_DestroyRenderer(this->ren);
	if(this->win)
		SDL_DestroyWindow(this->win);
	this->ren = nullptr;
	this->win = nullptr;

	Log::FormattedDebug("Window", "Destructor", "�Ҹ��� ȣ��");
}

int Window::SetWindow(WindowData w_dat) {
	SDL_SetWindowTitle(this->win, w_dat.title.c_str());
	int x, y;
	SDL_GetWindowPosition(this->win, &x, &y);
	int w, h;
	SDL_GetWindowSize(this->win, &w, &h);
	x += -w / 2 + w_dat.width / 2;
	y += -h / 2 + w_dat.height / 2;
	SDL_SetWindowPosition(this->win, x, y);
	SDL_SetWindowSize(this->win, w_dat.width, w_dat.height);
	this->fps = w_dat.fps;
	return 0;
}

int Window::Rendering() {
	thread th([&]() {this->__RenderingProcess__();});
	th.detach();
	return 0;
}

int Window::AddScene(Scene* scene, int pos) {
	if (find(this->scene_list.begin(), this->scene_list.end(), scene) != this->scene_list.end()) {
		return 1;
	}
	scene->RegisterRenderer(this->ren);
	if (pos == -1) {
		this->scene_list.push_back(scene);
	}
	else {
		this->scene_list.insert(this->scene_list.begin() + pos, scene);
	}
	return 0;
}

int Window::DeleteScene(Scene* scene) {
	auto iter = find(this->scene_list.begin(), this->scene_list.end(), scene);
	if (iter == this->scene_list.end()) {
		return 1;
	}
	(*iter)->Destroy();
	delete *iter;
	*iter = nullptr;
	this->scene_list.erase(iter);
	return 0;
}

void Window::__RenderingProcess__() {
	int prev = this->RunTime() * 1000;
	while (this->run) {
		int delta = (this->RunTime() * 1000 - prev);
		if (delta > 1000000 / this->fps) {
			prev += 1000000 / this->fps;
			SDL_RenderClear(this->ren);
			for (int i = this->scene_list.size() - 1; i >= 0; i--) {
				this->scene_list[i]->Rendering();
			}
			SDL_SetRenderDrawColor(this->ren, 0, 0, 0, 0);
			SDL_RenderPresent(this->ren);
			SDL_Delay(1000 / this->fps - 1);
		}
	}
	this->rendering_completed = true;
}

int Window::Destroy() {
	this->run = false;
	while (!this->rendering_completed);
	for (int i = 0; i < this->scene_list.size(); i++) {
		this->scene_list[i]->Destroy();
		delete this->scene_list[i];
		this->scene_list[i] = nullptr;
	}
	this->scene_list.clear();
	return 0;
}

EventType Window::PollEvent() {
	SDL_WaitEvent(&this->evt);
	switch (this->evt.type) {
	case SDL_QUIT:
		return QUIT;
	case SDL_KEYDOWN:
		for (auto& scene : this->scene_list) {
			scene->PushEvent(KEY_DOWN, this->evt.key.keysym.sym);
		}
		return KEY_DOWN;
	case SDL_KEYUP:
		for (auto& scene : this->scene_list) {
			scene->PushEvent(KEY_UP, this->evt.key.keysym.sym);
		}
		return KEY_UP;
	case SDL_MOUSEBUTTONDOWN:
		this->evt.motion.y *= -1;
		switch(this->evt.button.button) {
		case SDL_BUTTON_LEFT:
			for (auto& scene : this->scene_list) {
				scene->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_LEFT);
			}
			break;
		case SDL_BUTTON_RIGHT:
			for (auto& scene : this->scene_list) {
				scene->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_RIGHT);
			}
			break;
		case SDL_BUTTON_MIDDLE:
			for (auto& scene : this->scene_list) {
				scene->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_MIDDLE);
			}
			break;
		}
		return MOUSE_DOWN;
	case SDL_MOUSEBUTTONUP:
		this->evt.motion.y *= -1;
		switch (this->evt.button.button) {
			case SDL_BUTTON_LEFT:
				for (auto& scene : this->scene_list) {
					scene->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_LEFT);
				}
				break;
			case SDL_BUTTON_RIGHT:
				for (auto& scene : this->scene_list) {
					scene->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_RIGHT);
				}
				break;
			case SDL_BUTTON_MIDDLE:
				for (auto& scene : this->scene_list) {
					scene->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_MIDDLE);
				}
				break;
		}
		return MOUSE_UP;
	case SDL_MOUSEMOTION:
		this->evt.motion.y *= -1;
		for (auto& scene : this->scene_list) {
			scene->PushEvent(MOUSE_MOVE, this->evt.motion.x, -this->evt.motion.y, this->evt.motion.xrel, this->evt.motion.yrel, MOUSE_NONE);
		}
		return MOUSE_MOVE;
	default:
		return NONE;
	}
}

int Window::RunTime() {
	return clock() - this->windows_created_time;
}
