#include <iostream>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl();
static void runFile(std::string path);

int main(int argc, const char* argv[]) {
	auto args = parseArgs(argc, argv);

	if (args.empty()) {
		repl();
	}
	else if (args.size() == 1) {
		runFile(args[0]);
	}
	else {
		std::cerr << "Usage: clox (runs REPL) or clox [filepath]" << std::endl;
	}

	return 0;
}

static void repl() {
	VM vm{};

	char line[1024];
	while (true) {
		std::cout << "> ";
		if (!fgets(line, sizeof(line), stdin)) {
			std::cout << std::endl;
			break;
		}

		std::string str{ line };

		vm.interpret(str);
	}
}

static void runFile(std::string path) {
	VM vm{};

	auto source = readFile(path);
	auto result = vm.interpret(source);

	vm.free();

	if (result == InterpretResult::CompileTimeError) exit(65);
	if (result == InterpretResult::RuntimeError) exit(70);
}