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

	Token(TokenType t, std::string& s, int l) : type{ t }, text{ s }, line{ l } {}
	Token(TokenType t, const std::string& s, int l) : type{ t }, text{ s }, line{ l } {}
	Token(Scanner& scanner, TokenType t);
};

struct Scanner {
	std::string str;
	size_t start = 0;
	size_t current = 0;
	int line = 1;

	Scanner(std::string& source) : str{ source } { }

	Token scanToken();

	private:
	bool isAtEnd() {
		return current >= str.size();
	}

	Token makeToken(TokenType type) {
		return Token(type, str.substr(start, current - start), line);
	}

	Token errorToken(std::string message) {
		return Token(TokenType::Error, message, line);
	}

	char advance() {
		return str[current++];
	}

	bool match(char expected) {
		if (isAtEnd()) return false;
		if (str[current] != expected) return false;
		current++;
		return true;
	}

	char peek() {
		return str[current];
	}

	Token string();
	Token number();
	Token identifier();

	TokenType identifierType();
	TokenType checkKeyword(size_t sstart, const std::string& rest, TokenType type) {
		if (current - start == sstart + rest.size() && str.substr(start + sstart, rest.size()) == rest) return type;
		return TokenType::Identifier;
	}

	char peekNext() {
		if (isAtEnd()) return '\0';
		return str[current + 1];
	}

	void skipWhitespace() {
		while (true) {
			auto c = peek();
			switch (c) {
				case ' ':
				case '\r':
				case '\t':
					advance();
					break;
				case '\n':
					line++;
					advance();
					break;
				case '/':
					if (peekNext() == '/') {
						while (peek() != '\n' && !isAtEnd()) advance();
					} else {
						return;
					}
					break;
				default:
					return;
			}
		}
	}
};