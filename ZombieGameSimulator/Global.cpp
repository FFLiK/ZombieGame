#include "Global.h"

std::atomic<bool> Global::EVENT::CALLED = false;
std::atomic<bool> Global::EVENT::FINISHED = false;
std::atomic<bool> Global::EVENT::CREATE_TRIGGER = false;
std::atomic<bool> Global::EVENT::DELETE_TRIGGER = false;