#pragma once

#include <stdexcept>
#include <string>
#include <vector>

// Исключение для синтаксической ошибки в выражении
class SyntaxError : public std::runtime_error
{
public:
    SyntaxError(int offset, const char* what);

    int GetOffset() const { return m_offset; }

private:
    int m_offset{};
};

// Прочитать файл как массив строк
std::vector<std::string> ReadFile(const std::string& fileName);

enum class TokenType
{
    Invalid,    // Не используется
    Number,     // Целое или дробное число
    Identifier, // Идентификатор, состоящий из цифр, букв и/или знака подчеркивания
    Symbol      // Специальный символ (скобки, операторы, запятая)
};

// Токен: число, идентификатор, скобка, запятая или оператор
struct Token
{
    TokenType type{};
    int offset{};

    double number{};
    std::string identifier;
    char symbol{};

    bool IsOpenParenthesis() const { return type == TokenType::Symbol && symbol == '('; }
    bool IsCloseParenthesis() const { return type == TokenType::Symbol && symbol == ')'; }
    bool IsComma() const { return type == TokenType::Symbol && symbol == ','; }
    bool IsOperator() const { return type == TokenType::Symbol && !IsOpenParenthesis() && !IsCloseParenthesis() && !IsComma(); }
    bool IsIdentifier() const { return type == TokenType::Identifier; }
};

using TokenVec = std::vector<Token>;
using TokenVecIter = TokenVec::const_iterator;

// Разбить строку на токены
TokenVec Tokenize(const std::string& line);
