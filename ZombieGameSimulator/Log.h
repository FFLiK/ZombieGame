#pragma once
#include <string>
#include <iostream>
#include <Global.h>

class Log {
private:
	template<typename T, typename... Ts> static void _Print(T arg, Ts... args) {
		std::cout << arg << " ";
		Log::_Print(args...);
	}

	template<typename T> static void _Print(T arg) {
		std::cout << arg << std::endl;
	}

public:
	template<typename... Ts> static void FormattedDebug(std::string class_name, std::string function_name, Ts... args) {
		if (Global::SYSTEM::DEBUG_MODE) {
			std::cout << "[Debug Log] " + class_name + "::" + function_name + "(...)" + " - ";
			if (sizeof...(args)) Log::_Print(args...);
			else std::cout << std::endl;
		}
	}

	template<typename... Ts> static void Debug(Ts ...args) {
		if (Global::SYSTEM::DEBUG_MODE) {
			std::cout << "[Debug Log] ";
			if (sizeof...(args)) Log::_Print(args...);
			else std::cout << std::endl;
		}
	}

	template<typename... Ts> static void Error(Ts ...args) {
		std::cout << "[Error] ";
		std::cout << "\a";
		if (sizeof...(args)) Log::_Print(args...);
		else std::cout << std::endl;
		system("pause");
	}

	template<typename... Ts> static void System(Ts... args) {
		std::cout << "[System] ";
		if (sizeof...(args)) Log::_Print(args...);
		else std::cout << std::endl;
	}

	template<typename... Ts> static void Print(Ts... args) {
		if (sizeof...(args)) Log::_Print(args...);
		else std::cout << std::endl;
	}
};
