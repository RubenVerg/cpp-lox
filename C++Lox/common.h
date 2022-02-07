#pragma once

constexpr auto debug_traceExecution = true;

#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>

#undef EOF

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