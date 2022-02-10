#pragma once

#include "common.h"
#include "chunk.h"
#include "object.h"

struct Obj;
struct ObjString;

enum class InterpretResult {
	Ok,
	RuntimeError,
	CompileTimeError,
};

struct VM {
	Chunk chunk;
	size_t ip{ 0 };
	std::vector<Value> stack{};
	std::vector<std::shared_ptr<Obj>> objects{};
	std::unordered_map<std::string, std::shared_ptr<ObjString>> strings{};
	std::unordered_map<std::shared_ptr<ObjString>, Value> globals{};

	std::shared_ptr<ObjString> string(std::string str);

	InterpretResult interpret(std::string_view source);

	void push(Value value);

	std::optional<Value> pop();

	Value pop_unsafe();

	Value peek(size_t distance);

	void free();

	private:
	uint8_t readByte();
	uint16_t readShort();

	OpCode readOpCode();

	Value readConstant();

	template <typename F>
	InterpretResult binaryOperator(F f) {
		auto b = pop_unsafe().asNumber();
		auto a = pop_unsafe().asNumber();
		if (!a || !b) {
			runtimeError("Operands must be numbers.");
			return InterpretResult::RuntimeError;
		}
		push(Value{ f(a.value(), b.value()) });
		return InterpretResult::Ok;
	}

	void runtimeError(const std::string& format, ...);

	InterpretResult run();
};