#include "ExpressionBuilder.h"

namespace
{

// Если beginToken указывает на открывающую скобку, вернуть соответствующую ей закрывающую скобку.
// Используется для чтения произвольных выражений в скобках.
TokenVecIter FindMatchingParenthesis(TokenVecIter beginToken, TokenVecIter endToken)
{
    if (beginToken == endToken || !beginToken->IsOpenParenthesis())
        throw std::logic_error("Unexpected call of FindMatchingParenthesis");

    const int offset = beginToken->offset;

    int depth = 1;
    while (beginToken != endToken && depth > 0)
    {
        ++beginToken;
        if (beginToken == endToken)
            break;
        if (beginToken->IsOpenParenthesis())
            ++depth;
        if (beginToken->IsCloseParenthesis())
            --depth;
    }

    if (depth != 0)
        throw SyntaxError(offset, "Mismatched parenthesis");

    return beginToken;
}

// Возвращает следующую за beginToken запятую или закрывающую скобку.
// Используется для чтения параметров функции.
TokenVecIter FindFunctionArgumentEnd(TokenVecIter beginToken, TokenVecIter endToken)
{
    if (beginToken == endToken)
        throw std::logic_error("Unexpected call of FindMatchingParenthesis");

    const int offset = beginToken->offset;

    int depth = 1;
    while (beginToken != endToken)
    {
        if (beginToken->IsOpenParenthesis())
            ++depth;
        if (beginToken->IsCloseParenthesis())
            --depth;

        // Закрыто больше скобок чем открыто, значит найдена закрывающая скобка
        if (depth == 0)
            return beginToken;
        // Запятая не в скобках, значит найден следующий параметр
        if (depth == 1 && beginToken->IsComma())
            return beginToken;

        ++beginToken;
    }

    throw SyntaxError(offset, "Mismatched parenthesis in function call");
}

struct ExpressionNodeOrOperator
{
    int offset;
    ExpressionNodePtr node;
    char operator_{};

    ExpressionNodeOrOperator(int offset, ExpressionNodePtr node) : offset(offset), node(node) {}
    ExpressionNodeOrOperator(int offset, char operator_) : offset(offset), operator_(operator_) {}

    bool IsNode() const { return node != nullptr; }
    bool IsOperator() const { return node == nullptr; }
};

void CollapseExpression(std::vector<ExpressionNodeOrOperator>& nodesAndOperators)
{
    // Выбрать первый оператор с приоритетом не ниже, чем у оператора, следующего за ним
    size_t i = 1;
    while (i + 2 < nodesAndOperators.size())
    {
        if (GetPriority(nodesAndOperators[i].operator_) >= GetPriority(nodesAndOperators[i + 2].operator_))
            break;

        i += 2;
    }

    // Создать узел для этого оператора
    const auto offset = nodesAndOperators[i].offset;
    const auto op = nodesAndOperators[i].operator_;

    const auto operatorInfo = FindOperator(op);
    if (!operatorInfo)
    {
        throw SyntaxError(offset, "Unknown operator");
    }

    const std::vector<ExpressionNodePtr> children{ nodesAndOperators[i - 1].node, nodesAndOperators[i + 1].node };
    auto node = std::make_shared<OperatorExpressionNode>(children, operatorInfo->fn);

    auto begin = std::next(nodesAndOperators.begin(), i - 1);
    begin = nodesAndOperators.erase(begin, std::next(begin, 3));
    nodesAndOperators.emplace(begin, offset, node);
}

ExpressionNodePtr BuildExpressionForTokens(TokenVecIter beginToken, TokenVecIter endToken)
{
    if (beginToken == endToken)
        throw std::logic_error("There's no token to process");

    // Прочитать переменную или константу
    const auto numTokens = std::distance(beginToken, endToken);
    if (numTokens == 1)
    {
        switch (beginToken->type)
        {
        case TokenType::Number:
            return std::make_shared<ConstantExpressionNode>(beginToken->number);
        case TokenType::Identifier:
            return std::make_shared<VariableExpressionNode>(beginToken->identifier);
        default:
            throw SyntaxError(beginToken->offset, "Unexpected symbol");
        }
    }

    // Прочитать выражение
    std::vector<ExpressionNodeOrOperator> nodesAndOperators;
    while (beginToken != endToken)
    {
        // Прочитать символ оператора
        if (beginToken->IsOperator())
        {
            if (!nodesAndOperators.empty() && nodesAndOperators.back().IsOperator())
                throw SyntaxError(beginToken->offset, "Unexpected symbol");

            nodesAndOperators.emplace_back(beginToken->offset, beginToken->symbol);
            ++beginToken;
            continue;
        }

        // Если сейчас обрабатывается не оператор, предыдущий элемент должен быть оператором
        if (!nodesAndOperators.empty() && nodesAndOperators.back().IsNode())
            throw SyntaxError(beginToken->offset, "Unexpected symbol");

        // Прочитать под-выражение в скобках
        if (beginToken->IsOpenParenthesis())
        {
            auto closeParenthesis = FindMatchingParenthesis(beginToken, endToken);
            if (std::next(beginToken) == closeParenthesis)
                throw SyntaxError(closeParenthesis->offset, "Unexpected symbol");

            const auto node = BuildExpressionForTokens(std::next(beginToken), closeParenthesis);
            nodesAndOperators.emplace_back(beginToken->offset, node);
            beginToken = std::next(closeParenthesis);
            continue;
        }

        // Прочитать вызов функции
        const auto nextToken = std::next(beginToken);
        if (beginToken->IsIdentifier() && nextToken != endToken && nextToken->IsOpenParenthesis())
        {
            std::vector<ExpressionNodePtr> children;
            auto parameterBegin = std::next(nextToken);
            auto parameterEnd = parameterBegin;
            while (parameterBegin != endToken)
            {
                parameterEnd = FindFunctionArgumentEnd(parameterBegin, endToken);
                if (parameterBegin == parameterEnd)
                {
                    if (!children.empty())
                        throw SyntaxError(beginToken->offset, "Empty parameter");
                    break;
                }

                const auto node = BuildExpressionForTokens(parameterBegin, parameterEnd);
                children.push_back(node);

                if (parameterEnd->IsCloseParenthesis())
                    break;
                parameterBegin = std::next(parameterEnd);
            }

            const auto operatorInfo = FindOperator(beginToken->identifier);
            if (!operatorInfo)
            {
                throw SyntaxError(beginToken->offset, "Unknown function");
            }
            if (children.size() < operatorInfo->minArgs || children.size() > operatorInfo->maxArgs)
            {
                throw SyntaxError(beginToken->offset, "Unexpected number of function arguments");
            }

            auto node = std::make_shared<OperatorExpressionNode>(children, operatorInfo->fn);
            nodesAndOperators.emplace_back(beginToken->offset, node);
            beginToken = std::next(parameterEnd);
            continue;
        }

        // Прочитать одинокий токен
        const auto node = BuildExpressionForTokens(beginToken, std::next(beginToken));
        nodesAndOperators.emplace_back(beginToken->offset, node);
    }

    if (nodesAndOperators.back().IsOperator())
        throw SyntaxError(std::prev(endToken)->offset, "Unexpected symbol");

    // Обработать отдельно унарный оператор
    if (nodesAndOperators[0].IsOperator())
    {
        const auto offset = nodesAndOperators[0].offset;
        const char op = nodesAndOperators[0].operator_;
        const auto operand = nodesAndOperators[1].node;

        const auto operatorInfo = FindOperator(op);
        if (!operatorInfo)
        {
            throw SyntaxError(offset, "Unknown operator");
        }

        const std::vector<ExpressionNodePtr> children{ operand };
        auto node = std::make_shared<OperatorExpressionNode>(children, operatorInfo->fn);

        nodesAndOperators.erase(nodesAndOperators.begin(), std::next(nodesAndOperators.begin(), 2));
        nodesAndOperators.emplace(nodesAndOperators.begin(), offset, node);
    }

    // Операторы должны быть на нечетных местах, а уже готовые узлы дерева -- на четных
    for (size_t i = 0; i < nodesAndOperators.size(); ++i)
    {
        const auto& nodeOrOperator = nodesAndOperators[i];
        const bool oddPlace = i % 2 != 0;
        if (nodeOrOperator.IsOperator() != oddPlace)
            throw std::runtime_error("Unexpected expression sequence");
    }

    // Построить дерево операторов в соответствии с приоритетами
    while (nodesAndOperators.size() > 1)
        CollapseExpression(nodesAndOperators);

    return nodesAndOperators[0].node;
}

}

ExpressionNodePtr BuildExpression(const TokenVec& tokens)
{
    auto beginToken = tokens.begin();
    std::string outputVariable;

    if (tokens.size() > 2)
    {
        const auto& secondToken = tokens[1];
        if (secondToken.IsOperator() && secondToken.symbol == '=')
        {
            if (!tokens[0].IsIdentifier())
                throw SyntaxError(tokens[0].offset, "Identifier expected");

            beginToken = std::next(beginToken, 2);
            outputVariable = tokens[0].identifier;
        }
    }

    const auto expression = BuildExpressionForTokens(beginToken, tokens.end());
    return outputVariable.empty()
               ? expression
               : std::make_shared<AssignmentExpressionNode>(expression, outputVariable);
}
