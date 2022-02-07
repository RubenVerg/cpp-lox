#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

static uint8_t readByte(VM& vm) {
	return vm.chunk->code[vm.ip++];
}

static Value readConstant(VM& vm) {
	return vm.chunk->constants[readByte(vm)];
}

template <typename F>
static void binaryOperator(F f, VM& vm) {
	auto a = vm.pop_unsafe();
	auto b = vm.pop_unsafe();
	vm.push(f(a, b));
}

static InterpretResult run(VM& vm) {
	while (true) {
		if (debug_traceExecution) {
			std::cout << "          ";
			for (auto element : vm.stack) {
				std::cout << "[ ";
				printValue(element);
				std::cout << " ]";
			}
			std::cout << std::endl;
			disassembleInstruction(*vm.chunk, vm.ip);
		}

		auto instruction = asOpCode(readByte(vm));
		switch (instruction) {
			case OpCode::Constant:
			{
				auto constant = readConstant(vm);
				vm.push(constant);
				break;
			}
			case OpCode::Negate:
				vm.push(-vm.pop_unsafe());
				break;
			case OpCode::Add: binaryOperator([](Value a, Value b) { return a + b; }, vm); break;
			case OpCode::Subtract: binaryOperator([](Value a, Value b) { return a - b; }, vm); break;
			case OpCode::Multiply: binaryOperator([](Value a, Value b) { return a * b; }, vm); break;
			case OpCode::Divide: binaryOperator([](Value a, Value b) { return a / b; }, vm); break;;
			case OpCode::Return:
			{
				auto top = vm.pop();
				if (top) {
					printValue(top.value());
					std::cout << std::endl;
				}
				else {
					std::cout << "return with no values on the stack!" << std::endl;
				}
				return InterpretResult::Ok;
			}

			case OpCode::OPCODE_LEN:
				return InterpretResult::RuntimeError;
		}
	}
}

InterpretResult VM::interpret(std::string& source) {
	compile(source);
	return InterpretResult::Ok;
	// return run(*this);
}
