#include "scanner.h"

Token::Token(Scanner& scanner, TokenType t) : type{ t } {
	text = scanner.str.substr(scanner.start, scanner.current - scanner.start);
	line = scanner.line;
}

Token Scanner::string() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') line++;
		advance();
	}

	if (isAtEnd()) return errorToken("Unterminated string literal.");

	advance();
	return makeToken(TokenType::String);
}

Token Scanner::number() {
	while (isDigit(peek())) advance();

	if (peek() == '.' && isDigit(peekNext())) {
		advance();
		while (isDigit(peek())) advance();
	}

	return makeToken(TokenType::Number);
}

TokenType Scanner::identifierType() {
	switch (str[start]) {
		case 'a': return checkKeyword(1, "nd",    TokenType::And);
		case 'c': return checkKeyword(1, "lass",  TokenType::Class);
		case 'e': return checkKeyword(1, "lse",   TokenType::Else);
		case 'i': return checkKeyword(1, "f",     TokenType::If);
		case 'n': return checkKeyword(1, "il",    TokenType::Nil);
		case 'o': return checkKeyword(1, "r",     TokenType::Or);
		case 'p': return checkKeyword(1, "rint",  TokenType::Print);
		case 'r': return checkKeyword(1, "eturn", TokenType::Return);
		case 's': return checkKeyword(1, "uper",  TokenType::Super);
		case 'v': return checkKeyword(1, "ar",    TokenType::Var);
		case 'w': return checkKeyword(1, "hile",  TokenType::While);
		case 'f':
			if (current - start > 1) {
				switch (str[start + 1]) {
					case 'a': return checkKeyword(2, "lse", TokenType::False);
					case 'o': return checkKeyword(2, "r",   TokenType::For);
					case 'u': return checkKeyword(2, "n",   TokenType::Fun);
					default: return TokenType::Identifier;
				}
			} else return TokenType::Identifier;
		case 't':
			if (current - start > 1) {
				switch (str[start + 1]) {
					case 'h': return checkKeyword(2, "is", TokenType::This);
					case 'r': return checkKeyword(2, "ue", TokenType::True);
					default: return TokenType::Identifier;
				}
			} else return TokenType::Identifier;
		default: return TokenType::Identifier;
	}
}

TokenType Scanner::checkKeyword(size_t sstart, const std::string& rest, TokenType type) {
	if (current - start == sstart + rest.size() && str.substr(start + sstart, rest.size()) == rest) return type;
	return TokenType::Identifier;
}

void Scanner::skipWhitespace() {
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

char Scanner::peekNext() {
	if (isAtEnd()) return '\0';
	if (current == str.size()) return '\0';
	return str[current + 1];
}

Token Scanner::identifier() {
	while (isAlphaNumeric(peek())) advance();
	return makeToken(identifierType());
}

Token Scanner::scanToken() {
	skipWhitespace();

	start = current;

	if (isAtEnd()) return makeToken(TokenType::EOF);

	auto c = advance();

	if (isAlpha(c)) return identifier();
	if (isDigit(c)) return number();

	switch (c) {
		case '(': return makeToken(TokenType::LeftParen);
		case ')': return makeToken(TokenType::RightParen);
		case '{': return makeToken(TokenType::LeftBrace);
		case '}': return makeToken(TokenType::RightBrace);
		case ';': return makeToken(TokenType::Semicolon);
		case ',': return makeToken(TokenType::Comma);
		case '.': return makeToken(TokenType::Dot);
		case '+': return makeToken(TokenType::Plus);
		case '-': return makeToken(TokenType::Minus);
		case '*': return makeToken(TokenType::Star);
		case '/': return makeToken(TokenType::Slash);
		case '!': return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
		case '=': return makeToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
		case '>': return makeToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
		case '<': return makeToken(match('=') ? TokenType::LessEqual : TokenType::Less);
		case '"': return string();
	}

	return errorToken("Unexpected character.");
}

bool Scanner::isAtEnd() {
	// if ==, in C it'd be \0
	return current >= str.size();
}

Token Scanner::makeToken(TokenType type) {
	return Token(type, str.substr(std::min(start, str.size()), current - start), line);
}

Token Scanner::errorToken(std::string message) {
	return Token(TokenType::Error, message, line);
}

char Scanner::advance() {
	if (current >= str.size()) {
		current++;
		return '\0';
	}
	return str[current++];
}

bool Scanner::match(char expected) {
	if (isAtEnd()) return false;
	if (str[current] != expected) return false;
	current++;
	return true;
}

char Scanner::peek() {
	if (current >= str.size()) return '\0';
	return str[current];
}
