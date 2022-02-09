#include "compiler.h"
#include "object.h"
#include "scanner.h"
#include "vm.h"
#include "debug.h"

void Compiler::advance() {
	parser.previous = parser.current;

	while (true) {
		parser.current = scanner.scanToken();
		if (parser.current.type != TokenType::Error) break;

		errorAtCurrent(parser.current.text);
	}
}

void Compiler::emitByte(uint8_t byte) {
	currentChunk.addByte(byte, parser.previous.line);
}

void Compiler::emitOpCodeAndByte(OpCode code, uint8_t byte) {
	emitOpCode(code);
	emitByte(byte);
}

void Compiler::emitBytes(uint8_t byte1, uint8_t byte2) {
	emitByte(byte1);
	emitByte(byte2);
}

void Compiler::consume(TokenType type, const std::string& message) {
	if (parser.current.type == type) {
		advance();
	} else {
		errorAtCurrent(message);
	}
}

void Compiler::endCompilation() {
	emitReturn();
	if (debug_printCode && !parser.hadError) {
		disassembleChunk(currentChunk, "code");
	}
}

void Compiler::emitOpCode(OpCode code) {
	emitByte(asByte(code));
}

void Compiler::emitReturn() {
	emitOpCode(OpCode::Return);
}

uint8_t Compiler::makeConstant(Value value) {
	auto constant = currentChunk.addConstant(value);
	if (constant > std::numeric_limits<uint8_t>::max()) {
		error("Too many constants in one chunk.");
		return 0;
	}
	return uint8_t(constant);
}

void Compiler::emitConstant(Value value) {
	emitOpCodeAndByte(OpCode::Constant, makeConstant(value));
}

bool Compiler::check(TokenType type) {
	return parser.current.type == type;
}

bool Compiler::match(TokenType type) {
	if (!check(type)) return false;
	advance();
	return true;
}

void Compiler::parsePrecedence(Precedence precedence) {
	advance();
	auto prefixRule = rule(parser.previous.type).prefix;
	if (prefixRule == nullptr) {
		error("Expected an expression.");
		return;
	}

	auto canAssign = precedence <= Precedence::Assignment;
	(this->*prefixRule)(canAssign);

	while (precedence <= rule(parser.current.type).precedence) {
		advance();
		auto infixRule = rule(parser.previous.type).infix;
		(this->*infixRule)(canAssign);
	}

	if (canAssign && match(TokenType::Equal)) {
		error("Invalid assignment target.");
	}
}

void Compiler::expression() {
	parsePrecedence(Precedence::Assignment);
}

void Compiler::number(bool canAssign) {
	auto value = read_cast<double>(parser.previous.text);
	emitConstant(value);
}

void Compiler::string(bool canAssign) {
	auto value = parser.previous.text.substr(1, parser.previous.text.size() - 2);
	// lives forever, I think.
	auto string = std::make_shared<ObjString>(value);
	emitConstant(Value{ string });
}

void Compiler::grouping(bool canAssign) {
	expression();
	consume(TokenType::RightParen, "Expected ')' after expression.");
}

void Compiler::unary(bool canAssign) {
	auto operatorType = parser.previous.type;

	parsePrecedence(Precedence::Unary);

	switch (operatorType) {
		case TokenType::Bang: emitOpCode(OpCode::Not); break;
		case TokenType::Minus: emitOpCode(OpCode::Negate); break;
		default:
			unreachable();
	}
}

void Compiler::binary(bool canAssign) {
	auto operatorType = parser.previous.type;
	auto rule = Compiler::rule(operatorType);

	parsePrecedence(nextPrecedence(rule.precedence));

	switch (operatorType) {
		case TokenType::BangEqual: emitOpCode(OpCode::Equal); emitOpCode(OpCode::Not); break;
		case TokenType::EqualEqual: emitOpCode(OpCode::Equal); break;
		case TokenType::Greater: emitOpCode(OpCode::Greater); break;
		case TokenType::GreaterEqual: emitOpCode(OpCode::Less); emitOpCode(OpCode::Not); break;
		case TokenType::Less: emitOpCode(OpCode::Less); break;
		case TokenType::LessEqual: emitOpCode(OpCode::Greater); emitOpCode(OpCode::Not); break;
		case TokenType::Plus:  emitOpCode(OpCode::Add); break;
		case TokenType::Minus: emitOpCode(OpCode::Subtract); break;
		case TokenType::Star:  emitOpCode(OpCode::Multiply); break;
		case TokenType::Slash: emitOpCode(OpCode::Divide); break;
		default:
			unreachable();
	}
}

void Compiler::literal(bool canAssign) {
	switch (parser.previous.type) {
		case TokenType::False: emitOpCode(OpCode::False); break;
		case TokenType::True: emitOpCode(OpCode::True); break;
		case TokenType::Nil: emitOpCode(OpCode::Nil); break;
		default:
			unreachable();
	}
}

void Compiler::variable(bool canAssign) {
	namedVariable(parser.previous, canAssign);
}

void Compiler::namedVariable(Token& name, bool canAssign) {
	auto constant = identifierConstant(name);
	if (canAssign && match(TokenType::Equal)) {
		expression();
		emitOpCodeAndByte(OpCode::SetGlobal, constant);
	} else {
		emitOpCodeAndByte(OpCode::GetGlobal, constant);
	}
}

void Compiler::synchronize() {
	parser.panicMode = false;
	while (parser.current.type != TokenType::EOF) {
		if (parser.previous.type == TokenType::Semicolon) return;
		switch (parser.current.type) {
			case TokenType::Class:
			case TokenType::Fun:
			case TokenType::Var:
			case TokenType::For:
			case TokenType::While:
			case TokenType::If:
			case TokenType::Print:
			case TokenType::Return:
				return;
			default:
				advance();
		}
	}
}

void Compiler::varDeclaration() {
	auto global = parseVariable("Expected variable name.");

	if (match(TokenType::Equal)) {
		expression();
	} else {
		emitOpCode(OpCode::Nil);
	}

	consume(TokenType::Semicolon, "Expected ';' after variable declaration.");

	defineVariable(global);
}

uint8_t Compiler::parseVariable(const std::string& message) {
	consume(TokenType::Identifier, message);
	return identifierConstant(parser.previous);
}

uint8_t Compiler::identifierConstant(Token& name) {
	auto string = std::make_shared<ObjString>(name.text);
	return makeConstant(Value{ string });
}

void Compiler::defineVariable(uint8_t global) {
	emitOpCodeAndByte(OpCode::DefineGlobal, global);
}

void Compiler::declaration() {
	if (match(TokenType::Var)) {
		varDeclaration();
	} else {
		statement();
	}

	if (parser.panicMode) synchronize();
}

void Compiler::statement() {
	if (match(TokenType::Print)) {
		printStatement();
	} else {
		expressionStatement();
	}
}

void Compiler::printStatement() {
	expression();
	consume(TokenType::Semicolon, "Expected ';' after value.");
	emitOpCode(OpCode::Print);
}

void Compiler::expressionStatement() {
	expression();
	consume(TokenType::Semicolon, "Expected ';' after expression.");
	emitOpCode(OpCode::Drop);
}

std::optional<Chunk> Compiler::compile() {
	Chunk chunk{};
	currentChunk = chunk;

	advance();

	while (!match(TokenType::EOF)) {
		declaration();
	}

	endCompilation();

	if (parser.hadError) {
		return std::nullopt;
	} else {
		return std::make_optional(currentChunk);
	}
}

void Compiler::errorAtCurrent(const std::string& message) {
	errorAt(parser.current, message);
}

void Compiler::error(const std::string& message) {
	errorAt(parser.previous, message);
}

void Compiler::errorAt(Token& token, const std::string& message) {
	if (parser.panicMode) return;

	parser.panicMode = true;

	std::cerr << "[line " << token.line << "] Error";
	if (token.type == TokenType::EOF) {
		std::cerr << " at end";
	} else if (token.type == TokenType::Error) {} else {
		fprintf(stderr, " at '%.*s'", int(token.text.size()), token.text.c_str());
	}
	std::cerr << ": " << message << std::endl;
	parser.hadError = true;
}

bool validPrecedence(int precedence) {
	return precedence >= 0 && precedence < static_cast<int>(Precedence::PRECEDENCE_LEN);
}

Precedence nextPrecedence(Precedence precedence) {
	auto i = static_cast<int>(precedence) + 1;
	assert(validPrecedence(i), "nextPrecedence called on top precedence");
	return static_cast<Precedence>(i);
}

std::weak_ordering operator<=>(Precedence a, Precedence b) {
	return static_cast<int>(a) <=> static_cast<int>(b);
}
