#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <filesystem>
#include <cstdlib>
#include <Global.h>

#undef WriteConsole

class Log {
private:
	inline static std::ofstream logFile;        // normal log (no debug lines)
	inline static std::ofstream debugLogFile;   // includes ALL logs (debug + non-debug)
	inline static std::string logPath;
	inline static std::string debugLogPath;

	inline static std::mutex fileMutex;
	inline static std::mutex consoleMutex;

	// When enabled, Error(...) will throw and terminate the program.
	inline static std::atomic<bool> pauseOnError{ false };
	inline static std::atomic<bool> writeTimestamp{ true };
	inline static std::atomic<bool> writeThreadId{ true };
	inline static std::atomic<bool> isChanged{ false };

	// Time stamp formatter: YYYY-MM-DD HH:MM:SS.mmm
	static std::string NowTimestamp() {
		using namespace std::chrono;
		const auto now = system_clock::now();
		const auto t = system_clock::to_time_t(now);
		const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		std::tm tm{};
#if defined(_WIN32)
		localtime_s(&tm, &t);
#else
		localtime_r(&t, &tm);
#endif

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
			<< '.' << std::setfill('0') << std::setw(3) << ms.count();
		return oss.str();
	}

	static std::string Prefix(const char* level) {
		std::ostringstream oss;
		oss << '[' << level << ']';
		if (writeTimestamp.load(std::memory_order_relaxed)) {
			oss << ' ' << NowTimestamp();
		}
		if (writeThreadId.load(std::memory_order_relaxed)) {
			oss << " <" << std::this_thread::get_id() << '>';
		}
		oss << ' ';
		return oss.str();
	}

	static void WriteConsole(std::ostream& os, const std::string& line) {
		std::lock_guard<std::mutex> lock(consoleMutex);
		os << line << '\n';
	}

	// Write to files:
	// - If isDebug == false: write to BOTH normal log and debug-inclusive log.
	// - If isDebug == true : write ONLY to the debug-inclusive log.
	static void WriteFiles(const std::string& line, bool isDebug) {
		std::lock_guard<std::mutex> lock(fileMutex);
		if (isDebug) {
			if (debugLogFile.is_open()) {
				debugLogFile << line << '\n';
			}
		}
		else {
			if (logFile.is_open()) {
				logFile << line << '\n';
			}
			if (debugLogFile.is_open()) {
				debugLogFile << line << '\n';
			}
		}
	}

	template<typename... Ts>
	static std::string BuildLine(Ts&&... args) {
		std::ostringstream oss;
		// Fold expression to stream all args separated by a single space
		((oss << std::forward<Ts>(args) << ' '), ...);
		auto s = oss.str();
		if (!s.empty() && s.back() == ' ')
			s.pop_back();
		return s;
	}

	// Make a file-name-safe timestamp from NowTimestamp() (avoid ':', ' ' on Windows)
	static std::string SafeTimestampForFilename() {
		std::string ts = NowTimestamp();
		for (char& c : ts) {
			if (c == ':' || c == ' ') c = '-';
		}
		return ts;
	}

public:
	static void SetPauseOnError(bool enabled) { pauseOnError.store(enabled, std::memory_order_relaxed); }
	static void EnableTimestamp(bool enabled) { writeTimestamp.store(enabled, std::memory_order_relaxed); }
	static void EnableThreadId(bool enabled) { writeThreadId.store(enabled, std::memory_order_relaxed); }

	// Open two log files:
	// - Normal log (no debug lines)
	// - Debug-inclusive log (contains all lines including debug)
	// If directory does not exist, create it.
	static void OpenLogFile() {
		namespace fs = std::filesystem;

		// Build base directory
		std::string baseDir;
		if (Global::SYSTEM::USE_APPDATA) {
			if (const char* appdata = std::getenv("APPDATA")) {
#if defined(_WIN32)
				baseDir = std::string(appdata) + "\\" + Global::SYSTEM::NAME + "\\";
#else
				baseDir = std::string(appdata) + "/" + Global::SYSTEM::NAME + "/";
#endif
			}
		}

		// Build full directory path (base + LOG_PATH)
		fs::path dirPath = fs::path(baseDir) / fs::path(Global::SYSTEM::LOG_PATH);

		// Ensure directory exists (create if missing)
		std::error_code ec;
		if (!fs::exists(dirPath)) {
			if (!fs::create_directories(dirPath, ec) && ec) {
				std::lock_guard<std::mutex> consoleLock(consoleMutex);
				std::cerr << "[Error] Failed to create log directory: " << dirPath.string() << " (" << ec.message() << ")\n";
			}
		}

		// Build filenames with a file-system-safe timestamp
		const std::string baseName = SafeTimestampForFilename();
		const fs::path normalPath = dirPath / fs::path(baseName + Global::SYSTEM::LOG_EXTENSION);
		const fs::path debugPath = dirPath / fs::path(baseName + ".debug" + Global::SYSTEM::LOG_EXTENSION);

		{
			std::lock_guard<std::mutex> lock(fileMutex);

			if (logFile.is_open())       logFile.close();
			if (debugLogFile.is_open())  debugLogFile.close();

			logFile.clear();
			debugLogFile.clear();

			logPath = normalPath.string();
			debugLogPath = debugPath.string();

			logFile.open(logPath, std::ios::out | std::ios::app);        // create if not exist
			debugLogFile.open(debugLogPath, std::ios::out | std::ios::app);

			if (!logFile) {
				std::lock_guard<std::mutex> consoleLock(consoleMutex);
				std::cerr << "[Error] Failed to open log file: " << logPath << '\n';
			}
			if (!debugLogFile) {
				std::lock_guard<std::mutex> consoleLock(consoleMutex);
				std::cerr << "[Error] Failed to open debug log file: " << debugLogPath << '\n';
			}
		}
	}

	static void FlushLogFile() {
		std::lock_guard<std::mutex> lock(fileMutex);
		bool ok = false;
		if (logFile.is_open()) {
			logFile.flush();
			ok = true;
		}
		if (debugLogFile.is_open()) {
			debugLogFile.flush();
			ok = true;
		}
		if (ok) {
			WriteConsole(std::cout, std::string("[System] Log files flushed."));
		}
		else {
			WriteConsole(std::cerr, std::string("[Error] Log files are not open."));
		}
	}

#if defined(__cpp_attributes)
	[[deprecated("Use FlushLogFile() instead")]]
#endif
	static void SaveLogFile() {
		FlushLogFile();
	}

	static void CloseLogFile() {
		std::lock_guard<std::mutex> lock(fileMutex);
		if (logFile.is_open())      logFile.close();
		if (debugLogFile.is_open()) debugLogFile.close();
	}

	// File-only logging for non-debug lines (writes to both files)
	template<typename... Ts>
	static void FileLog(Ts&&... args) {
		WriteFiles(BuildLine(std::forward<Ts>(args)...), /*isDebug=*/false);
	}
	static void FileLog() { WriteFiles(std::string{}, /*isDebug=*/false); } // empty line

	// Console + file with level prefixes
	// - Console: only when DEBUG_MODE
	// - Files: debug-inclusive file ALWAYS, normal file NEVER for debug lines
	template<typename... Ts>
	static void FormattedDebug(const std::string& class_name, const std::string& function_name, Ts&&... args) {
		const std::string head = Prefix("Debug Log") + class_name + "::" + function_name + "(...) - ";
		const std::string tail = sizeof...(args) ? BuildLine(std::forward<Ts>(args)...) : std::string{};
		const std::string line = head + tail;

		if (Global::SYSTEM::DEBUG_MODE) {
			WriteConsole(std::cout, line);
		}
		WriteFiles(line, /*isDebug=*/true);
	}

	template<typename... Ts>
	static void Debug(Ts&&... args) {
		const std::string head = Prefix("Debug Log");
		const std::string tail = sizeof...(args) ? BuildLine(std::forward<Ts>(args)...) : std::string{};
		const std::string line = head + tail;

		if (Global::SYSTEM::DEBUG_MODE) {
			WriteConsole(std::cout, line);
		}
		WriteFiles(line, /*isDebug=*/true);
	}

	template<typename... Ts>
	static void Error(Ts&&... args) {
		const std::string head = Prefix("Error");
		const std::string tail = sizeof...(args) ? BuildLine(std::forward<Ts>(args)...) : std::string{};
		const std::string line = head + tail;

		WriteConsole(std::cerr, line);
		WriteFiles(line, /*isDebug=*/false);

		if (pauseOnError.load(std::memory_order_relaxed)) {
			{
				std::lock_guard<std::mutex> lock(fileMutex);
				if (logFile.is_open())      logFile.flush(), logFile.close();
				if (debugLogFile.is_open()) debugLogFile.flush(), debugLogFile.close();
			}
			try {
				throw std::runtime_error(line);
			}
			catch (...) {
				std::terminate();
			}
		}
	}

	template<typename... Ts>
	static void System(Ts&&... args) {
		const std::string head = Prefix("System");
		const std::string tail = sizeof...(args) ? BuildLine(std::forward<Ts>(args)...) : std::string{};
		const std::string line = head + tail;

		WriteConsole(std::cout, line);
		WriteFiles(line, /*isDebug=*/false);
	}

	// Raw print (no level prefix). Writes to console and both files (non-debug).
	template<typename... Ts>
	static void Print(Ts&&... args) {
		const std::string line = sizeof...(args) ? BuildLine(std::forward<Ts>(args)...) : std::string{};
		WriteConsole(std::cout, line);
		WriteFiles(line, /*isDebug=*/false);
	}
	static void Print() { // explicit empty-line overload
		WriteConsole(std::cout, std::string{});
		WriteFiles(std::string{}, /*isDebug=*/false);
	}
};