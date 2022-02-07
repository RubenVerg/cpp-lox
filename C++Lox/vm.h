#pragma once

#include "common.h"
#include "chunk.h"

enum class InterpretResult {
	Ok,
	RuntimeError,
	CompileTimeError,
};

struct VM {
	Chunk* chunk = nullptr;
	size_t ip = 0;
	std::vector<Value> stack{};

	InterpretResult interpret(std::string& source);

	void push(Value value) {
		stack.push_back(value);
	}

	std::optional<Value> pop() {
		if (stack.empty()) {
			return std::nullopt;
		}
		else {
			auto result = stack.back();
			stack.pop_back();
			return std::make_optional(result);
		}
	}

	Value pop_unsafe() {
		return pop().value();
	}

	Value peek(size_t distance) {
		return stack[stack.size() - 1 - distance];
	}

	private:
	uint8_t readByte() {
		return chunk->code[ip++];
	}

	OpCode readOpCode() {
		return asOpCode(readByte());
	}

	Value readConstant() {
		return chunk->constants[readByte()];
	}

	template <typename F>
	InterpretResult binaryOperator(F f) {
		auto b = pop_unsafe().asNumber();
		auto a = pop_unsafe().asNumber();
		if (!a || !b) {
			runtimeError("Operands must be numbers.");
			return InterpretResult::RuntimeError;
		}
		push(f(a.value(), b.value()));
		return InterpretResult::Ok;
	}

	void runtimeError(const std::string& format, ...) {
		auto str = format.c_str();
		va_list args;
		va_start(args, str);
		vfprintf(stderr, str, args);
		va_end(args);
		std::cerr << std::endl;

		std::cerr << "[ line " << chunk->lines[ip] << "] in script" << std::endl;
		stack.clear();
	}

	InterpretResult run();
};