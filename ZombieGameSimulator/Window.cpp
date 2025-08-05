#include "Window.h"
#include <ctime>
#include <thread>
#include <Log.h>
#include <Texture.h>
#include <Resources.h>

Window::Window(WindowData w_dat) {
	Log::FormattedDebug("Window", "Constructor", "Calling constructor of Window with title: " + w_dat.title + ", width: " + std::to_string(w_dat.width) + ", height: " + std::to_string(w_dat.height) + ", fps: " + std::to_string(w_dat.fps));
	if (this->run) {
		return;
	}
	this->run = true;
	this->rendering_completed = false;
	this->win = SDL_CreateWindow(w_dat.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w_dat.width, w_dat.height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	this->ren = SDL_CreateRenderer(this->win, -1, SDL_RENDERER_ACCELERATED);
	this->fps = w_dat.fps;
	this->windows_created_time = clock();

	this->frame_cnt = 0;
	this->frame_time = this->windows_created_time;
}

Window::~Window() {
	if (this->ren)
		SDL_DestroyRenderer(this->ren);
	if(this->win)
		SDL_DestroyWindow(this->win);
	this->ren = nullptr;
	this->win = nullptr;

	Log::FormattedDebug("Window", "Destructor", "Calling destructor of Window and destroying window and renderer");
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

int Window::Execute() {
	thread th([&]() {this->__Process__();});
	th.detach();
	return 0;
}

int Window::AddScene(Scene* scene, int pos) {
	this->scene_mtx.lock();
	if (find(this->scene_list.begin(), this->scene_list.end(), scene) != this->scene_list.end()) {
		this->scene_mtx.unlock();
		return 1;
	}
	scene->RegisterRenderer(this->ren);
	scene->ProcessInit();
	if (pos == -1) {
		this->scene_list.push_back(scene);
	}
	else {
		this->scene_list.insert(this->scene_list.begin() + pos, scene);
	}
	this->scene_mtx.unlock();
	return 0;
}

int Window::DeleteScene(Scene* scene) {
	this->scene_mtx.lock();
	auto iter = find(this->scene_list.begin(), this->scene_list.end(), scene);
	if (iter == this->scene_list.end()) {
		this->scene_mtx.unlock();
		return 1;
	}
	delete *iter;
	*iter = nullptr;
	this->scene_list.erase(iter);
	this->scene_mtx.unlock();
	return 0;
}

void Window::__Process__() {
	if (Global::SYSTEM::TEXTURE_RENDERING) {
		this->scene_mtx.lock();
		InitLoadTextureLibrary(this->ren);
		Resources::InitResources(this->ren);
		this->scene_mtx.unlock();
	}
	int prev = this->RunTime() * 1000;
	while (this->run) {
		this->scene_mtx.lock();
		int delta = (this->RunTime() * 1000 - prev);
		if (delta > 1000000 / this->fps) {
			prev += 1000000 / this->fps;
			SDL_RenderClear(this->ren);
			for (int i = this->scene_list.size() - 1; i >= 0; i--) {
				this->scene_list[i]->Rendering();
			}
			SDL_SetRenderDrawColor(this->ren, 0, 0, 0, 0);
			SDL_RenderPresent(this->ren);
			this->frame_cnt++;
		}
		this->event_mtx.lock();
		for (int i = this->scene_list.size() - 1; i >= 0; i--) {
			this->scene_list[i]->__Process__();
		}
		this->event_mtx.unlock();
		this->scene_mtx.unlock();
	}
	if (Global::SYSTEM::TEXTURE_RENDERING) {
		Resources::QuitResources();
		QuitLoadTextureLibrary();
	}
	this->rendering_completed = true;
}

int Window::Destroy() {
	this->run = false;
	while (!this->rendering_completed);
	for (int i = 0; i < this->scene_list.size(); i++) {
		delete this->scene_list[i];
		this->scene_list[i] = nullptr;
	}
	this->scene_list.clear();
	return 0;
}

EventType Window::PollEvent() {
	SDL_WaitEvent(&this->evt);
	lock_guard<recursive_mutex> lock(this->event_mtx);
	switch (this->evt.type) {
	case SDL_QUIT:
		return QUIT;
	case SDL_KEYDOWN:
		this->scene_list[0]->PushEvent(KEY_DOWN, this->evt.key.keysym.sym);
		return KEY_DOWN;
	case SDL_KEYUP:
		this->scene_list[0]->PushEvent(KEY_UP, this->evt.key.keysym.sym);
		if (this->evt.key.keysym.sym == SDLK_ESCAPE) {
			this->run = false;
		}
		else if (this->evt.key.keysym.sym == SDLK_F11) {
			if (!this->is_full_screen) {
				//SDL_SetWindowFullscreen(this->win, 0);
				this->is_full_screen = true;
			}
			else {
				//SDL_SetWindowFullscreen(this->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
				this->is_full_screen = false;
			}
		}
		else if (this->evt.key.keysym.sym == SDLK_F12) {
			double fps = this->frame_cnt / ((this->RunTime() - this->frame_time) / 1000.0);
			Log::System("Current FPS: ", fps);
			this->frame_cnt = 0;
			this->frame_time = this->RunTime();
		}
		return KEY_UP;
	case SDL_MOUSEBUTTONDOWN:
		this->evt.motion.y *= -1;
		switch(this->evt.button.button) {
		case SDL_BUTTON_LEFT:
			this->scene_list[0]->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_LEFT);
			break;
		case SDL_BUTTON_RIGHT:
			this->scene_list[0]->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_RIGHT);
			break;
		case SDL_BUTTON_MIDDLE:
			this->scene_list[0]->PushEvent(MOUSE_DOWN, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_MIDDLE);
			break;
		}
		return MOUSE_DOWN;
	case SDL_MOUSEBUTTONUP:
		this->evt.motion.y *= -1;
		switch (this->evt.button.button) {
			case SDL_BUTTON_LEFT:
				this->scene_list[0]->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_LEFT);
				break;
			case SDL_BUTTON_RIGHT:
				this->scene_list[0]->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_RIGHT);
				break;
			case SDL_BUTTON_MIDDLE:
				this->scene_list[0]->PushEvent(MOUSE_UP, this->evt.button.x, -this->evt.button.y, 0, 0, MOUSE_MIDDLE);
				break;
		}
		return MOUSE_UP;
	case SDL_MOUSEMOTION:
		this->evt.motion.y *= -1;
		this->scene_list[0]->PushEvent(MOUSE_MOVE, this->evt.motion.x, -this->evt.motion.y, this->evt.motion.xrel, this->evt.motion.yrel, MOUSE_NONE);
		return MOUSE_MOVE;
	default:
		return NONE;
	}
}

int Window::RunTime() {
	return clock() - this->windows_created_time;
}
