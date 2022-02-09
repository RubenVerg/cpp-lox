#pragma once

#include "common.h"
#include "value.h"

enum class OpCode : uint8_t {
	Constant,
	Nil,
	True,
	False,
	Not,
	Negate,
	Add,
	Subtract,
	Multiply,
	Divide,
	Equal,
	Less,
	Greater,
	Return,
	Drop,
	Print,
	DefineGlobal,
	GetGlobal,
	SetGlobal,

	OPCODE_LEN
};

inline uint8_t asByte(OpCode code) {
	return static_cast<uint8_t>(code);
}

inline bool validOpCode(uint8_t byte) {
	return byte < asByte(OpCode::OPCODE_LEN);
}

inline OpCode asOpCode(uint8_t byte) {
	assert(validOpCode(byte), "Casting unknown byte " << byte << " to OpCode");
	return static_cast<OpCode>(byte);
}

struct Chunk {
	std::vector<uint8_t> code;
	std::vector<Value> constants;
	std::vector<int> lines;

	void addInstruction(OpCode instruction, int line) {
		addByte(asByte(instruction), line);
	}

	void addByte(uint8_t byte, int line) {
		code.push_back(byte);
		lines.push_back(line);
	}

	size_t addConstant(Value value) {
		constants.push_back(value);
		return constants.size() - 1;
	}
};