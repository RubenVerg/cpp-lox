#include "scanner.h"

static bool isAtEnd(Scanner& scanner) {
    return scanner.current >= scanner.str.size();
}

static Token makeToken(Scanner& scanner, TokenType type) {
    return Token(type, scanner.str.substr(scanner.start, scanner.current - scanner.start), scanner.line);
}

static Token errorToken(Scanner& scanner, const std::string& message) {
    return Token(TokenType::Error, message, scanner.line);
}

static char advance(Scanner& scanner) {
    return scanner.str[scanner.current++];
}

static bool match(Scanner& scanner, char expected) {
    if (isAtEnd(scanner)) return false;
    if (scanner.str[scanner.current] != expected) return false;
    scanner.current++;
    return true;
}

static char peek(Scanner& scanner) {
    return scanner.str[scanner.current];
}

static bool skipWhitespace(Scanner& scanner) {
    while (true) {
        auto c = peek(scanner);
        switch (c) {
        case ' ':
        case '\r':
        case '\t':
            advance(scanner);
            break;
        case '\n':
            scanner.line+
        default:
            return;
        }
    }
}

Token Scanner::scanToken() {
    auto scanner = *this;

    skipWhitespace(scanner);

    start = current;
    
    if (isAtEnd(scanner)) return makeToken(scanner, TokenType::EOF);

    auto c = advance(scanner);
    switch (c) {
    case '(': return makeToken(scanner, TokenType::LeftParen);
    case ')': return makeToken(scanner, TokenType::RightBrace);
    case '{': return makeToken(scanner, TokenType::LeftBrace);
    case '}': return makeToken(scanner, TokenType::RightBrace);
    case ';': return makeToken(scanner, TokenType::Semicolon);
    case ',': return makeToken(scanner, TokenType::Comma);
    case '.': return makeToken(scanner, TokenType::Dot);
    case '+': return makeToken(scanner, TokenType::Plus);
    case '-': return makeToken(scanner, TokenType::Minus);
    case '*': return makeToken(scanner, TokenType::Star);
    case '/': return makeToken(scanner, TokenType::Slash);
    case '!': return makeToken(scanner, match(scanner, '=') ? TokenType::BangEqual : TokenType::Bang);
    case '=': return makeToken(scanner, match(scanner, '=') ? TokenType::EqualEqual : TokenType::Equal);
    case '>': return makeToken(scanner, match(scanner, '=') ? TokenType::GreaterEqual : TokenType::Greater);
    case '<': return makeToken(scanner, match(scanner, '=') ? TokenType::LessEqual : TokenType::Less);
    }

    return errorToken(*this, "Unexpected character.");
}
