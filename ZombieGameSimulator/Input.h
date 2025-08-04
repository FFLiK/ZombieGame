#pragma once
#include <string>
#include <iostream>
#include <Global.h>

class Input {
public:
	static std::string GetInputString(const std::string &prompt) {
		std::string input;
		std::cout << "> " << prompt << " : ";
		std::getline(std::cin, input);
		return input;
	}

	static std::string GetInputString(const std::string &prompt, const std::string &default_value) {
		std::string input = GetInputString(prompt + " (default: " + default_value + "): ");
		if (input.empty()) {
			return default_value;
		}
		return input;
	}

	static int GetInputInt(const std::string &prompt) {
		int input;
		std::cout << "> " << prompt << " : ";
		while (!(std::cin >> input)) {
			std::cin.clear(); // clear the error flag
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard invalid input
			std::cout << "> Invalid input. Please enter an integer: ";
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard the newline character
		return input;
	}

	static int GetInputInt(const std::string& prompt, int min, int max) {
		int input;
		do {
			input = GetInputInt(prompt + " (range: " + std::to_string(min) + " - " + std::to_string(max) + ")");
			if (input < min || input > max) {
				std::cout << "> Input out of range. Please try again." << std::endl;
			}
		} while (input < min || input > max);
		return input;
	}
};
