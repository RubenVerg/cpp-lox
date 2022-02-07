#include "compiler.h"
#include "scanner.h"
#include "debug.h"

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

void Compiler::advance() {
	parser.previous = parser.current;

	while (true) {
		parser.current = scanner.scanToken();
		if (parser.current.type != TokenType::Error) break;

		errorAtCurrent(parser.current.text);
	}
}

void Compiler::endCompilation() {
	emitReturn();
	if (debug_printCode && !parser.hadError) {
		disassembleChunk(*currentChunk, "code");
	}
}

void Compiler::parsePrecedence(Precedence precedence) {
	advance();
	auto prefixRule = rule(parser.previous.type).prefix;
	if (prefixRule == nullptr) {
		error("Expected an expression.");
		return;
	}
	(this->*prefixRule)();

	while (precedence <= rule(parser.current.type).precedence) {
		advance();
		auto infixRule = rule(parser.previous.type).infix;
		(this->*infixRule)();
	}
}

void Compiler::expression() {
	parsePrecedence(Precedence::Assignment);
}

void Compiler::number() {
	auto value = read_cast<double>(parser.previous.text);
	emitConstant(value);
}

void Compiler::grouping() {
	expression();
	consume(TokenType::RightParen, "Expected ')' after expression.");
}

void Compiler::unary() {
	auto operatorType = parser.previous.type;

	parsePrecedence(Precedence::Unary);

	switch (operatorType) {
		case TokenType::Minus: emitOpCode(OpCode::Negate); break;
		default:
			assert(false);
	}
}

void Compiler::binary() {
	auto operatorType = parser.previous.type;
	auto rule = Compiler::rule(operatorType);

	parsePrecedence(nextPrecedence(rule.precedence));

	switch (operatorType) {
		case TokenType::Plus:  emitOpCode(OpCode::Add); break;
		case TokenType::Minus: emitOpCode(OpCode::Subtract); break;
		case TokenType::Star:  emitOpCode(OpCode::Multiply); break;
		case TokenType::Slash: emitOpCode(OpCode::Divide); break;
		default:
			assert(false);
	}
}

std::unordered_map<TokenType, ParseRule> Compiler::rules{
	{TokenType::LeftParen,    ParseRule(&Compiler::grouping, nullptr,           Precedence::None)},
	{TokenType::RightParen,   ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::LeftBrace,    ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::RightBrace,   ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Comma,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Dot,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Minus,        ParseRule(&Compiler::unary,    &Compiler::binary, Precedence::Sum)},
	{TokenType::Plus,         ParseRule(nullptr,             &Compiler::binary, Precedence::Sum)},
	{TokenType::Slash,        ParseRule(nullptr,             &Compiler::binary, Precedence::Product)},
	{TokenType::Star,         ParseRule(nullptr,             &Compiler::binary, Precedence::Product)},
	{TokenType::Semicolon,    ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Bang,         ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::BangEqual,    ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Equal,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::EqualEqual,   ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Greater,      ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::GreaterEqual, ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Less,         ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::LessEqual,    ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Identifier,   ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::String,       ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Number,       ParseRule(&Compiler::number,   nullptr,           Precedence::None)},
	{TokenType::And,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Or,           ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Class,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::If,           ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Else,         ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::For,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::While,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Fun,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::False,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::True,         ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Nil,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Print,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Return,       ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::This,         ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Super,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Var,          ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::Error,        ParseRule(nullptr,             nullptr,           Precedence::None)},
	{TokenType::EOF,          ParseRule(nullptr,             nullptr,           Precedence::None)},
};

std::optional<Chunk> Compiler::compile() {
	Chunk chunk{};
	currentChunk = &chunk;

	advance();
	expression();
	consume(TokenType::EOF, "Expected end of expression.");

	endCompilation();

	if (parser.hadError) {
		return std::nullopt;
	} else {
		return std::make_optional(chunk);
	}
}