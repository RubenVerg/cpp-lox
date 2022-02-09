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

	std::shared_ptr<ObjString> string(std::string str) {
		if (strings.contains(str)) {
			return strings[str];
		} else {
			auto string = std::make_shared<ObjString>(str);
			objects.push_back(std::static_pointer_cast<Obj>(string));
			strings[str] = string;
			return string;
		}
	}

	InterpretResult interpret(std::string_view source);

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

	void free();

	private:
	uint8_t readByte() {
		return chunk.code[ip++];
	}

	OpCode readOpCode() {
		return asOpCode(readByte());
	}

	Value readConstant() {
		auto constant = chunk.constants[readByte()];
		if (constant.isObj()) {
			if (constant.asObjUnsafe().get()->isString()) {
				return Value{ string(constant.asObjUnsafe().get()->asStringUnsafe()) };
			} else {
				objects.push_back(constant.asObjUnsafe());
			};
		}
		return constant;
	}

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

	void runtimeError(const std::string& format, ...) {
		auto str = format.c_str();
		va_list args;
		va_start(args, str);
		vfprintf(stderr, str, args);
		va_end(args);
		std::cerr << std::endl;

		std::cerr << "[line " << chunk.lines[ip] << "] in script" << std::endl;
		stack.clear();
	}

	InterpretResult run();
};