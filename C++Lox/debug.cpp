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
	chunk.constants[constant].print();
	std::cout << "'" << std::endl;
	return index + 2;
}

static size_t byteInstruction(std::string name, Chunk& chunk, size_t index) {
	auto slot = chunk.code[index + 1];
	printf("%-16s %4d", name.c_str(), slot);
	std::cout << std::endl;
	return index + 2;
}

static size_t jumpInstruction(std::string name, bool backwards, Chunk& chunk, size_t index) {
	auto jump = static_cast<size_t>(chunk.code[index + 1]) << 8;
	jump |= chunk.code[index + 2];
	printf("%-16s %4zd -> %zd", name.c_str(), index, index + 3 + (backwards ? -1 : 1) * jump);
	std::cout << std::endl;
	return index + 3;
}

size_t disassembleInstruction(Chunk& chunk, size_t index) {
	printf("%04d ", int(index));

	if (index > 0 && chunk.lines[index] == chunk.lines[index - 1]) {
		std::cout << "   | ";
	} else {
		printf("%4d ", chunk.lines[index]);
	}

	auto instruction = chunk.code[index];
	if (static_cast<uint8_t>(OpCode::OPCODE_LEN) <= instruction) {
		fprintf(stderr, "Unknown opcode %x", instruction);
		std::cerr << std::endl;
		return index + 1;
	} else {
		switch (asOpCode(instruction)) {
			case OpCode::Constant:
				return constantInstruction("constant", chunk, index);
			case OpCode::Nil:
				return simpleInstruction("nil", index);
			case OpCode::True:
				return simpleInstruction("true", index);
			case OpCode::False:
				return simpleInstruction("false", index);
			case OpCode::Not:
				return simpleInstruction("!", index);
			case OpCode::Negate:
				return simpleInstruction("unary -", index);
			case OpCode::Add:
				return simpleInstruction("+", index);
			case OpCode::Subtract:
				return simpleInstruction("-", index);
			case OpCode::Multiply:
				return simpleInstruction("*", index);
			case OpCode::Divide:
				return simpleInstruction("/", index);
			case OpCode::Equal:
				return simpleInstruction("==", index);
			case OpCode::Greater:
				return simpleInstruction(">", index);
			case OpCode::Less:
				return simpleInstruction("<", index);
			case OpCode::Return:
				return simpleInstruction("return", index);
			case OpCode::Drop:
				return simpleInstruction("drop", index);
			case OpCode::Print:
				return simpleInstruction("print", index);
			case OpCode::DefineGlobal:
				return constantInstruction("define global", chunk, index);
			case OpCode::GetGlobal:
				return constantInstruction("get global", chunk, index);
			case OpCode::SetGlobal:
				return constantInstruction("set global", chunk, index);
			case OpCode::GetLocal:
				return byteInstruction("get local", chunk, index);
			case OpCode::SetLocal:
				return byteInstruction("set local", chunk, index);
			case OpCode::ConditionalJump:
				return jumpInstruction("jump if false", false, chunk, index);
			case OpCode::Jump:
				return jumpInstruction("jump", false, chunk, index);
			case OpCode::JumpBack:
				return jumpInstruction("jump back", true, chunk, index);
			default:
				unreachable();
				return 0;
		}
	}
}