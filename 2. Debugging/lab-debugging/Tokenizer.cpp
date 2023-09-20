#include "Tokenizer.h"

#include <fstream>

namespace
{

// Список специальных символов, допустимых в выражении (кроме символа комментария)
static const std::string sc_specialSymbols = "()+-*/=,";

bool IsSpace(char ch) { return std::isspace(static_cast<unsigned char>(ch)); }
bool IsDigit(char ch) { return std::isdigit(static_cast<unsigned char>(ch)); }
bool IsAlnum(char ch) { return std::isalnum(static_cast<unsigned char>(ch)); }
bool IsAlnumOrUnderscore(char ch) { return IsAlnum(ch) || ch == '_'; }

}

SyntaxError::SyntaxError(int offset, const char* what)
    : std::runtime_error(std::to_string(offset) + ": " + what), m_offset(offset)
{
}

std::vector<std::string> ReadFile(const std::string& fileName)
{
    std::vector<std::string> result;
    std::string line;

    std::ifstream file(fileName);
    while (std::getline(file, line))
        result.push_back(line);

    return result;
}

TokenVec Tokenize(const std::string& line)
{
    TokenVec result;

    auto ptr = line.begin();
    const auto end = line.end();
    while (ptr != end)
    {
        const auto offset = static_cast<int>(ptr - line.begin());

        // Пропустить комментарии
        if (*ptr == '#')
        {
            break;
        }

        // Пропустить пробелы
        if (IsSpace(*ptr))
        {
            ++ptr;
            continue;
        }

        // Прочитать число
        if (IsDigit(*ptr))
        {
            // Прочитать целую часть
            double integerPart = 0.0;
            while (ptr != end && IsDigit(*ptr))
            {
                const int digit = *ptr - '0';
                integerPart *= 10.0;
                integerPart += digit;
                ++ptr;
            }

            // Прочитать дробную часть
            double fractionalPart = 0.0;
            if (ptr != end && *ptr == '.')
            {
                ++ptr;
                // После точки должна идти цифра
                if (ptr == end || !IsDigit(*ptr))
                    throw SyntaxError(offset, "Unexpected symbol");

                double scale = 1.0;
                while (ptr != end && IsDigit(*ptr))
                {
                    const int digit = *ptr - '0';
                    scale *= 0.1;
                    fractionalPart += digit * scale;
                    ++ptr;
                }
            }

            // Добавить токен
            Token token{ TokenType::Number, offset };
            token.number = integerPart + fractionalPart;
            result.push_back(token);
            continue;
        }

        // Прочитать идентификатор
        if (IsAlnumOrUnderscore(*ptr))
        {
            std::string identifier;
            while (ptr != end && IsAlnumOrUnderscore(*ptr))
            {
                identifier += *ptr;
                ++ptr;
            }

            // Добавить токен
            Token token{ TokenType::Identifier, offset };
            token.identifier = identifier;
            result.push_back(token);
            continue;
        }

        // Прочитать специальный символ
        if (sc_specialSymbols.find(*ptr) != std::string::npos)
        {
            Token token{ TokenType::Symbol, offset };
            token.symbol = *ptr;
            result.push_back(token);
            ++ptr;
            continue;
        }

        // Неизвестный символ
        throw SyntaxError(offset, "Unexpected symbol");
    }

    return result;
}
