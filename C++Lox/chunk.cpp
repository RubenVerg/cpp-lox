#include "chunk.h"

uint8_t asByte(OpCode code) {
	return static_cast<uint8_t>(code);
}

bool validOpCode(uint8_t byte) {
	return byte < asByte(OpCode::OPCODE_LEN);
}

OpCode asOpCode(uint8_t byte) {
	assert(validOpCode(byte), "Casting unknown byte " << byte << " to OpCode");
	return static_cast<OpCode>(byte);
}

void Chunk::addInstruction(OpCode instruction, int line) {
	addByte(asByte(instruction), line);
}

void Chunk::addByte(uint8_t byte, int line) {
	code.push_back(byte);
	lines.push_back(line);
}

size_t Chunk::addConstant(Value value) {
	constants.push_back(value);
	return constants.size() - 1;
}
