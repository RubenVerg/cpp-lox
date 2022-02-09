#pragma once

#ifdef _DEBUG
constexpr auto debug_printCode = true;
constexpr auto debug_traceExecution = true;
constexpr auto debug_logFrees = true;

#define assert(expr, err) do {\
	if (!expr) {\
		std::cerr << "Assertion failed: " << err << std::endl;\
		std::cerr << "At " << __FILE__ << ":" << __LINE__;\
		__pragma(warning(suppress: 4130))\
		if (__func__ != nullptr) {\
			std::cerr << " (in " << __func__ << ")";\
		}\
		std::cerr << std::endl;\
		abort();\
  }\
} while (false)
#else
constexpr auto debug_printCode = false;
constexpr auto debug_traceExecution = false;
constexpr auto debug_logFrees = false;

#define assert(expr, err) ((void)0)
#endif

#define unreachable() assert(false, "Unreachable code")

#include <cstddef>
#include <cstdint>
#include <cstdarg>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <limits>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#undef EOF

using namespace std::literals;

inline std::vector<std::string> parseArgs(int argc, const char* argv[]) {
	std::vector<std::string> out{};
	for (size_t i = 1; i < argc; i++) {
		std::string str{ argv[i] };
		out.push_back(str);
	}
	return out;
}

inline std::string readFile(const std::string& path) {
	std::ifstream input_file{ path };
	if (!input_file.is_open()) {
		std::cerr << "Could not open file " << path << "." << std::endl;
		exit(74);
	}
	return std::string(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
}

inline bool isDigit(char c) {
	return '0' <= c && c <= '9';
}

inline bool isAlpha(char c) {
	return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

inline bool isAlphaNumeric(char c) {
	return isAlpha(c) || isDigit(c);
}

template<typename To, typename From> inline To read_cast(const From& input) {
	std::stringstream stream;
	To output;

	stream << input;
	stream >> output;

	return output;
}