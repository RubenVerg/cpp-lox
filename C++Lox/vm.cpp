#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"
#include "object.h"

#define ReturnIfError(value) do {\
  auto result = value;\
  if (result != InterpretResult::Ok) {\
    return result;\
	}\
} while (false)

void VM::runtimeError(const std::string & format, ...) {
	auto str = format.c_str();
	va_list args;
	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
	std::cerr << std::endl;

	std::cerr << "[line " << chunk.lines[ip] << "] in script" << std::endl;
	stack.clear();
}

InterpretResult VM::run() {
	while (true) {
		if (debug_traceExecution) {
			std::cout << "          ";
			for (auto element : stack) {
				std::cout << "[ ";
				element.print();
				std::cout << " ]";
			}
			std::cout << std::endl;
			disassembleInstruction(chunk, ip);
		}

		auto instruction = readOpCode();
		switch (instruction) {
			case OpCode::Constant:
			{
				auto constant = readConstant();
				push(constant);
				break;
			}
			case OpCode::Nil:
				push(Value{});
				break;
			case OpCode::True:
				push(Value{ true });
				break;
			case OpCode::False:
				push(Value{ false });
				break;
			case OpCode::Not:
				push(Value{ !pop_unsafe().castToBool() });
				break;
			case OpCode::Negate:
			{
				auto value = pop_unsafe().asNumber();
				if (!value) {
					runtimeError("Operand must be a number.");
					return InterpretResult::RuntimeError;
				}
				push(Value{ -value.value() });
				break;
			}
			case OpCode::Add:
			{
				if (peek(0).isObj() && peek(0).asObjUnsafe()->isString() && peek(1).isObj() && peek(1).asObjUnsafe()->isString()) {
					auto b = pop_unsafe().asObjUnsafe()->asStringUnsafe();
					auto a = pop_unsafe().asObjUnsafe()->asStringUnsafe();
					auto str = string(a + b);
					push(Value{ str });
				} else if (peek(0).isNumber() && peek(1).isNumber()) {
					auto b = pop_unsafe().asNumberUnsafe();
					auto a = pop_unsafe().asNumberUnsafe();
					push(Value{ a + b });
				} else {
					runtimeError("Operands must be either two numbers or two strings.");
					return InterpretResult::RuntimeError;
				}
				break;
			}
			case OpCode::Subtract: ReturnIfError(binaryOperator([] (double a, double b) { return a - b; })); break;
			case OpCode::Multiply: ReturnIfError(binaryOperator([] (double a, double b) { return a * b; })); break;
			case OpCode::Divide: ReturnIfError(binaryOperator([] (double a, double b) { return a / b; })); break;
			case OpCode::Equal:
			{
				auto b = pop_unsafe();
				auto a = pop_unsafe();
				push(Value{ a == b });
				break;
			}
			case OpCode::Greater: ReturnIfError(binaryOperator([] (double a, double b) { return a > b; })); break;
			case OpCode::Less: ReturnIfError(binaryOperator([] (double a, double b) { return a < b; })); break;
			case OpCode::Return: return InterpretResult::Ok;
			case OpCode::Drop: pop_unsafe(); break;
			case OpCode::DefineGlobal:
			{
				auto str = readConstant().asObjUnsafe().get()->asStringUnsafe();
				auto name = strings[str];
				if (globals.contains(name)) {
					runtimeError("Global variable %s already declared.", str.c_str());
					return InterpretResult::RuntimeError;
				}
				globals[name] = peek(0);
				pop_unsafe();
				break;
			}
			case OpCode::GetGlobal:
			{
				auto str = readConstant().asObjUnsafe().get()->asStringUnsafe();
				auto name = strings[str];
				if (!globals.contains(name)) {
					runtimeError("Unknown global variable %s.", str.c_str());
					return InterpretResult::RuntimeError;
				}
				push(globals[name]);
				break;
			}
			case OpCode::SetGlobal:
			{
				auto str = readConstant().asObjUnsafe().get()->asStringUnsafe();
				auto name = strings[str];
				if (!globals.contains(name)) {
					runtimeError("Cannot assign to unknown global variable %s.", str.c_str());
					return InterpretResult::RuntimeError;
				}
				globals[name] = peek(0);
				break;
			}
			case OpCode::GetLocal:
			{
				auto slot = readByte();
				push(stack[slot]);
				break;
			}
			case OpCode::SetLocal:
			{
				auto slot = readByte();
				stack[slot] = peek(0);
				break;
			}
			case OpCode::ConditionalJump:
			{
				auto offset = readShort();
				if (!peek(0).castToBool()) ip += offset;
				break;
			}
			case OpCode::Jump:
			{
				auto offset = readShort();
				ip += offset;
				break;
			}
			case OpCode::JumpBack:
			{
				auto offset = readShort();
				ip -= offset;
				break;
			}
			case OpCode::Print:
			{
				pop_unsafe().print();
				std::cout << std::endl;
				break;
			}

			case OpCode::OPCODE_LEN:
				return InterpretResult::CompileTimeError;
		}
	}
}

std::shared_ptr<ObjString> VM::string(std::string str) {
	if (strings.contains(str)) {
		return strings[str];
	} else {
		auto string = std::make_shared<ObjString>(str);
		objects.push_back(std::static_pointer_cast<Obj>(string));
		strings[str] = string;
		return string;
	}
}

InterpretResult VM::interpret(std::string_view source) {
	Compiler compiler{ source };

	auto newChunk = compiler.compile();

	if (!newChunk) {
		return InterpretResult::CompileTimeError;
	}

	chunk = newChunk.value();
	ip = 0;

	return run();
}

void VM::push(Value value) {
	stack.push_back(value);
}

std::optional<Value> VM::pop() {
	if (stack.empty()) {
		return std::nullopt;
	} else {
		auto result = stack.back();
		stack.pop_back();
		return result;
	}
}

Value VM::pop_unsafe() {
	return pop().value();
}

Value VM::peek(size_t distance) {
	return stack[stack.size() - 1 - distance];
}

void VM::free() {
	if (debug_logFrees) {
		std::cout << "Freeing " << objects.size() << " objects." << std::endl;
	}
	objects.clear();
}

uint8_t VM::readByte() {
	return chunk.code[ip++];
}

uint16_t VM::readShort() {
	ip += 2;
	return static_cast<uint16_t>(chunk.code[ip - 2]) << 8 | chunk.code[ip - 1];
}

OpCode VM::readOpCode() {
	return asOpCode(readByte());
}

Value VM::readConstant() {
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
