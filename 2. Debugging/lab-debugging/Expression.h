#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

using OperatorFunction = std::function<double(const std::vector<double>& args)>;

// Структура, которая описывает оператор: как математические операции, так и функции
struct OperatorInfo
{
    std::string name;
    int priority{};
    unsigned minArgs{};
    unsigned maxArgs{};
    OperatorFunction fn;

    OperatorInfo(const std::string& name) : name(name) {}

    OperatorInfo(const std::string& name, int priority, unsigned minArgs, unsigned maxArgs, OperatorFunction fn)
        : name(name), priority(priority), minArgs(minArgs), maxArgs(maxArgs), fn(fn)
    {
    }

    bool operator<(const OperatorInfo& rhs) const { return name < rhs.name; }
};

// Найти оператор по символу или идентификатору
const OperatorInfo* FindOperator(const std::string& op);
const OperatorInfo* FindOperator(char op);

// Операторы с более высоким приоритетом будут обработаны первые.
int GetPriority(char ch);

// Элемент дерева выражения
class ExpressionNode
{
public:
    virtual double Compute() const = 0;
};

using ExpressionNodePtr = std::shared_ptr<ExpressionNode>;

// Константа в дереве выражения
class ConstantExpressionNode : public ExpressionNode
{
public:
    explicit ConstantExpressionNode(double value) : m_value(value) {}
    double Compute() const override { return m_value; }

private:
    double m_value{};
};

// Переменная в дереве выражения
class VariableExpressionNode : public ExpressionNode
{
public:
    explicit VariableExpressionNode(const std::string& identifier) : m_identifier(identifier) {}
    double Compute() const override;

private:
    std::string m_identifier;
};

// Оператор в дереве выражения
class OperatorExpressionNode : public ExpressionNode
{
public:
    explicit OperatorExpressionNode(std::vector<ExpressionNodePtr> children, OperatorFunction op)
        : m_children(std::move(children)), m_op(std::move(op))
    {
    }

    double Compute() const override;

private:
    std::vector<ExpressionNodePtr> m_children;
    OperatorFunction m_op;
};

// Специальный узел для оператора присваивания
class AssignmentExpressionNode : public ExpressionNode
{
public:
    explicit AssignmentExpressionNode(ExpressionNodePtr child, const std::string& variable)
        : m_child(child), m_variable(variable)
    {
    }

    double Compute() const override;

private:
    ExpressionNodePtr m_child;
    std::string m_variable;
};
