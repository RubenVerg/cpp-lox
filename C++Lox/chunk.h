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
	GetLocal,
	SetLocal,

	OPCODE_LEN
};

uint8_t asByte(OpCode code);

bool validOpCode(uint8_t byte);

OpCode asOpCode(uint8_t byte);

struct Chunk {
	std::vector<uint8_t> code;
	std::vector<Value> constants;
	std::vector<int> lines;

	void addInstruction(OpCode instruction, int line);

	void addByte(uint8_t byte, int line);

	size_t addConstant(Value value);
};