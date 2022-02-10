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
#include <ios>
#include <variant>

#undef EOF

using namespace std::literals;

std::vector<std::string> parseArgs(int argc, const char* argv[]);

std::string readFile(const std::string & path);

bool isDigit(char c);

bool isAlpha(char c);

bool isAlphaNumeric(char c);

template<typename To, typename From> inline To read_cast(const From& input) {
	std::stringstream stream;
	To output;

	stream << input;
	stream >> output;

	return output;
}