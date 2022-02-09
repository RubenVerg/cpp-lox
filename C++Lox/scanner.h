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

struct Scanner;

struct Token {
	TokenType type;
	std::string text;
	int line;

	Token(TokenType t, std::string_view s, int l) : type{ t }, text{ s }, line{ l } {}
	Token(Scanner& scanner, TokenType t);
};

struct Scanner {
	std::string_view str;
	size_t start = 0;
	size_t current = 0;
	int line = 1;

	Scanner(std::string_view source) : str{ source } { }

	Token scanToken();

	private:
	bool isAtEnd();

	Token makeToken(TokenType type);

	Token errorToken(std::string message);

	char advance();

	bool match(char expected);

	char peek();
	char peekNext();

	Token string();
	Token number();
	Token identifier();

	TokenType identifierType();
	TokenType checkKeyword(size_t sstart, const std::string& rest, TokenType type);

	void skipWhitespace();
};