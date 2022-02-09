#pragma once

#include "common.h"
#include "chunk.h"
#include "scanner.h"

struct VM;

struct Parser {
	Token current{ TokenType::Error, "", -1 };
	Token previous{ TokenType::Error, "", -1 };
	bool hadError{ false };
	bool panicMode{ false };
};

enum class Precedence : uint8_t {
	None,
	Assignment, // =
	Or,         // or
	And,        // and
	Equality,   // == !=
	Comparison, // < <= > >=
	Sum,        // + -
	Product,    // * /
	Unary,      // ! -
	Call,       // . ()
	Primary,

	PRECEDENCE_LEN
};

inline bool validPrecedence(int precedence) {
	return precedence >= 0 && precedence < static_cast<int>(Precedence::PRECEDENCE_LEN);
}

inline Precedence nextPrecedence(Precedence precedence) {
	auto i = static_cast<int>(precedence) + 1;
	assert(validPrecedence(i), "nextPrecedence called on top precedence");
	return static_cast<Precedence>(i);
}

inline std::weak_ordering operator<=>(Precedence a, Precedence b) {
	return static_cast<int>(a) <=> static_cast<int>(b);
}

struct Compiler;

using ParseFn = void (Compiler::*)(bool canAssign);

struct ParseRule {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;

	ParseRule(ParseFn p, ParseFn i, Precedence pr) : precedence{ pr } {
		prefix = p;
		infix = i;
	}
};

struct Compiler {
	std::string_view source;
	Scanner scanner{ source };
	Parser parser{};
	Chunk currentChunk;

	std::optional<Chunk> compile();

	private:
	void errorAtCurrent(const std::string& message) {
		errorAt(parser.current, message);
	}

	void error(const std::string& message) {
		errorAt(parser.previous, message);
	}

	void errorAt(Token& token, const std::string& message);

	void advance();

	void emitByte(uint8_t byte) {
		currentChunk.addByte(byte, parser.previous.line);
	}

	void emitOpCodeAndByte(OpCode code, uint8_t byte) {
		emitOpCode(code);
		emitByte(byte);
	}

	void emitBytes(uint8_t byte1, uint8_t byte2) {
		emitByte(byte1);
		emitByte(byte2);
	}

	void consume(TokenType type, const std::string& message) {
		if (parser.current.type == type) {
			advance();
		} else {
			errorAtCurrent(message);
		}
	}

	void endCompilation();

	void emitOpCode(OpCode code) {
		emitByte(asByte(code));
	}

	void emitReturn() {
		emitOpCode(OpCode::Return);
	}

	uint8_t makeConstant(Value value) {
		auto constant = currentChunk.addConstant(value);
		if (constant > std::numeric_limits<uint8_t>::max()) {
			error("Too many constants in one chunk.");
			return 0;
		}
		return uint8_t(constant);
	}

	void emitConstant(Value value) {
		emitOpCodeAndByte(OpCode::Constant, makeConstant(value));
	}

	bool check(TokenType type) {
		return parser.current.type == type;
	}

	bool match(TokenType type) {
		if (!check(type)) return false;
		advance();
		return true;
	}

	void parsePrecedence(Precedence precedence);

	void expression();

	void number(bool canAssign);

	void string(bool canAssign);

	void grouping(bool canAssign);

	void unary(bool canAssign);

	void binary(bool canAssign);

	void literal(bool canAssign);

	void variable(bool canAssign);
	void namedVariable(Token& name, bool canAssign);

	void printStatement();

	void expressionStatement();

	void statement();

	void varDeclaration();
	uint8_t parseVariable(const std::string& message);
	uint8_t identifierConstant(Token& name);
	void defineVariable(uint8_t global);

	void declaration();

	void synchronize();

	static std::unordered_map<TokenType, ParseRule> rules;

	public:
	static ParseRule rule(TokenType type) {
		return rules.at(type);
	}
};