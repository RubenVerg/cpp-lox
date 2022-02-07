#include "debug.h"

void disassembleChunk(Chunk& chunk, std::string name) {
	std::cout << "== " << name << " ==" << std::endl;
	for (size_t index = 0; index < chunk.code.size();) {
		index = disassembleInstruction(chunk, index);
	}
}

static size_t simpleInstruction(std::string name, size_t index) {
	std::cout << name << std::endl;
	return index + 1;
}

static size_t constantInstruction(std::string name, Chunk& chunk, size_t index) {
	auto constant = chunk.code[index + 1];
	printf("%-16s %4d '", name.c_str(), constant);
	printValue(chunk.constants[constant]);
	std::cout << "'" << std::endl;
	return index + 2;
}

size_t disassembleInstruction(Chunk& chunk, size_t index) {
	printf("%04d ", int(index));

	if (index > 0 && chunk.lines[index] == chunk.lines[index - 1]) {
		std::cout << "   | ";
	} else {
		printf("%4d ", chunk.lines[index]);
	}

	auto instruction = chunk.code[index];
	if (!validOpCode(instruction)) {
		std::cerr << "Unknown opcode " << instruction << std::endl;
		return index + 1;
	}
	else {
		switch (asOpCode(instruction)) {
		case OpCode::Constant:
			return constantInstruction("constant", chunk, index);
		case OpCode::Negate:
			return simpleInstruction("negate", index);
		case OpCode::Add:
			return simpleInstruction("add", index);
		case OpCode::Subtract:
			return simpleInstruction("subtract", index);
		case OpCode::Multiply:
			return simpleInstruction("multiply", index);
		case OpCode::Divide:
			return simpleInstruction("divide", index);
		case OpCode::Return:
			return simpleInstruction("return", index);
		default:
			std::cerr << "Unknown opcode " << instruction << std::endl;
			return index + 1;
		}
	}
}