#include "common.h"
#include "vm.h"
#include "debug.h"
#include "compiler.h"

InterpretResult VM::run() {
	while (true) {
		if (debug_traceExecution) {
			std::cout << "          ";
			for (auto element : stack) {
				std::cout << "[ ";
				printValue(element);
				std::cout << " ]";
			}
			std::cout << std::endl;
			disassembleInstruction(*chunk, ip);
		}

		auto instruction = readOpCode();
		switch (instruction) {
			case OpCode::Constant:
			{
				auto constant = readConstant();
				push(constant);
				break;
			}
			case OpCode::Negate:
				auto value = pop_unsafe().asNumber();
				if (!value) {
					runtimeError("Operand must be a number.");
					return;
				}
				push(Value(-value.value()));
				break;
				// TODO support the runtime errors "thrown"
			case OpCode::Add: binaryOperator([](double a, double b) { return a + b; }); break;
			case OpCode::Subtract: binaryOperator([](double a, double b) { return a - b; }); break;
			case OpCode::Multiply: binaryOperator([](double a, double b) { return a * b; }); break;
			case OpCode::Divide: binaryOperator([](double a, double b) { return a / b; }); break;;
			case OpCode::Return:
			{
				auto top = pop();
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
	Compiler compiler{ source };

	auto newChunk = compiler.compile();

	if (!newChunk) {
		return InterpretResult::CompileTimeError;
	}

	chunk = &newChunk.value();
	ip = 0;

	return run();
}
