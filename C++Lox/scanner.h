#pragma once

#include "common.h"


enum class TokenType {
	LeftParen, RightParen,
	LeftBrace, RightBrace,
	Comma, Dot, Minus, Plus,
	Semicolon, Slash, Star,
	Bang, BangEqual,
	Equal, EqualEqual,
	Greater, GreaterEqual,
	Less, LessEqual,
	Identifier, String, Number,
	And, Class, Else, False,
	For, Fun, If, Nil, Or,
	Print, Return, Super, This,
	True, Var, While,
	Error, EOF
};

struct Token {
	TokenType type;
	std::string text;
	int line;

	Token(const TokenType t, const std::string& s, int l) : type{ t }, text{ s }, line{ l } {}
	Token(Scanner& scanner, const TokenType t) : type{ t } {
		text = scanner.str.substr(scanner.start, scanner.current - scanner.start);
		line = scanner.line;
	}
};

struct Scanner {
	std::string str;
	size_t start = 0;
	size_t current = 0;
	int line = 1;

	Scanner(std::string& source) : str{ source } { }

	Token scanToken();
};