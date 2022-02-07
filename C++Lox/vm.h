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
};