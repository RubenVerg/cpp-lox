#include "common.h"

std::vector<std::string> parseArgs(int argc, const char* argv[]) {
	std::vector<std::string> out{};
	for (size_t i = 1; i < argc; i++) {
		std::string str{ argv[i] };
		out.push_back(str);
	}
	return out;
}

std::string readFile(const std::string& path) {
	std::ifstream input_file{ path };
	if (!input_file.is_open()) {
		std::cerr << "Could not open file " << path << "." << std::endl;
		exit(74);
	}
	return std::string(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
}

bool isDigit(char c) {
	return '0' <= c && c <= '9';
}

bool isAlpha(char c) {
	return c == '_' || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isAlphaNumeric(char c) {
	return isAlpha(c) || isDigit(c);
}
