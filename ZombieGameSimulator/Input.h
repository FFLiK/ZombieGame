#pragma once
#include <string>
#include <iostream>
#include <Global.h>
#include <chrono>

class Input {
private:
	static double input_duration;

public:
	static std::string GetInputString(const std::string &prompt) {
		auto start = std::chrono::steady_clock::now();
		std::string input;
		std::cout << "> " << prompt << " : ";
		std::getline(std::cin, input);
		auto end = std::chrono::steady_clock::now();
		input_duration += std::chrono::duration<double>(end - start).count();
		return input;
	}

	static int GetInputInt(const std::string &prompt, bool inner_called = false) {
		auto start = std::chrono::steady_clock::now();
		int input;
		std::cout << "> " << prompt << " : ";
		while (!(std::cin >> input)) {
			std::cin.clear(); // clear the error flag
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
			std::cout << "> Invalid input. Please enter an integer: ";
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard the newline character
		auto end = std::chrono::steady_clock::now();
		if (!inner_called) {
			input_duration += std::chrono::duration<double>(end - start).count();
		}
		return input;
	}

	static int GetInputInt(const std::string& prompt, int min, int max) {
		auto start = std::chrono::steady_clock::now();
		int input;
		do {
			input = GetInputInt(prompt + " (range: " + std::to_string(min) + " - " + std::to_string(max) + ")", true);
			if (input < min || input > max) {
				std::cout << "> Input out of range. Please try again." << std::endl;
			}
		} while (input < min || input > max);
		auto end = std::chrono::steady_clock::now();
		input_duration += std::chrono::duration<double>(end - start).count();
		return input;
	}

	static double GetInputDuration() {
		return input_duration;
	}
};
