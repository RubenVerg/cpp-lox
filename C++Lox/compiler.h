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

bool validPrecedence(int precedence);

Precedence nextPrecedence(Precedence precedence);

std::weak_ordering operator<=>(Precedence a, Precedence b);

struct Compiler;

using ParseFn = void (Compiler::*)(bool canAssign);

struct ParseRule {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;

	ParseRule(ParseFn p, ParseFn i, Precedence pr) : precedence{ pr }, infix{ i }, prefix{ p } {}
};

struct Local {
	Token name;
	int depth;
};

struct Compiler {
	static ParseRule rule(TokenType type);

	std::string_view source;
	Scanner scanner{ source };
	Parser parser{};
	Chunk currentChunk;
	std::vector<Local> locals{};
	int scopeDepth{ 0 };

	std::optional<Chunk> compile();

	private:
	static std::unordered_map<TokenType, ParseRule> rules;

	void errorAtCurrent(const std::string& message);
	void error(const std::string& message);
	void errorAt(Token& token, const std::string& message);

	void advance();

	void emitByte(uint8_t byte);
	void emitOpCodeAndByte(OpCode code, uint8_t byte);
	void emitBytes(uint8_t byte1, uint8_t byte2);
	void emitOpCode(OpCode code);
	void emitReturn();
	void emitConstant(Value value);

	void consume(TokenType type, const std::string& message);

	void endCompilation();

	uint8_t makeConstant(Value value);

	bool check(TokenType type);
	bool match(TokenType type);

	void beginScope();
	void endScope();

	void parsePrecedence(Precedence precedence);

	void expression();

	size_t emitJump(OpCode code);
	void patchJump(size_t jump);

	void emitLoop(size_t start);

	void number(bool canAssign);
	void string(bool canAssign);
	void grouping(bool canAssign);
	void unary(bool canAssign);
	void binary(bool canAssign);
	void literal(bool canAssign);

	void andExpr(bool canAssign);
	void orExpr(bool canAssign);

	void variable(bool canAssign);
	void namedVariable(Token& name, bool canAssign);
	void declareVariable();
	void addLocal(Token& name);
	std::optional<size_t> resolveLocal(Token& name);

	void printStatement();

	void ifStatement();
	
	void whileStatement();

	void forStatement();

	void expressionStatement();
	void block();

	void statement();

	void varDeclaration();
	uint8_t parseVariable(const std::string& message);
	uint8_t identifierConstant(Token& name);
	void defineVariable(uint8_t global);
	void markInitialized();

	void declaration();

	void synchronize();
};