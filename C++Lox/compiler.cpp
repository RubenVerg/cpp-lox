#include "compiler.h"
#include "scanner.h"

void compile(std::string& source) {
	Scanner scanner{ source };

	int line = -1;

	while (true) {
		auto token = scanner.scanToken();
		if (token.line != line) {
			printf("%4d ", token.line);
			line = token.line;
		}
		else {
			std::cout << "   | ";
		}
		printf("%2d '%.*s'", token.type, token.text.size(), token.text.c_str());
		std::cout << std::endl;

		if (token.type == Token::EOF) break;
	}
}