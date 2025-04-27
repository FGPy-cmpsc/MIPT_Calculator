#pragma once
#include <stack>

#include "AbstractToken.hpp"
#include "Exceptions.hpp"
#include "ExprInPolishNotation.hpp"
#include "OperandToken.hpp"
#include "OperatorToken.hpp"

template <typename Type>
class Calculator {
 public:
  static Type CalculateExpr(const std::string& expr);

 private:
  static void ProcessTokens(const std::vector<AbstractToken*>& tokens,
                            std::stack<OperandToken<Type>*>& eval_stack);

  static void ProcessSingleToken(AbstractToken* token,
                                 std::stack<OperandToken<Type>*>& eval_stack);

  static void ProcessBinary(OperatorToken<Type, true>* op_token_binary,
                            std::stack<OperandToken<Type>*>& eval_stack);

  static void ProcessUnary(OperatorToken<Type, false>* op_token_unary,
                           std::stack<OperandToken<Type>*>& eval_stack);

  static void ValidateStackFinalState(
      std::stack<OperandToken<Type>*>& eval_stack);

  static Type ExtractFinalResult(std::stack<OperandToken<Type>*>& eval_stack);

  static void CleanupStack(std::stack<OperandToken<Type>*>& eval_stack);
};

template <typename Type>
Type Calculator<Type>::CalculateExpr(const std::string& expr) {
  ExprInPolishNotation<Type> expr_p_n(expr);
  const auto& tokens = expr_p_n.GetTokens();

  if (tokens.empty()) {
    throw InvalidExpr();
  }

  std::stack<OperandToken<Type>*> eval_stack;
  ProcessTokens(tokens, eval_stack);
  return ExtractFinalResult(eval_stack);
}

template <typename Type>
void Calculator<Type>::ProcessTokens(
    const std::vector<AbstractToken*>& tokens,
    std::stack<OperandToken<Type>*>& eval_stack) {
  for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    AbstractToken* token = *it;
    ProcessSingleToken(token, eval_stack);
  }

  ValidateStackFinalState(eval_stack);
}

template <typename Type>
void Calculator<Type>::ProcessSingleToken(
    AbstractToken* token, std::stack<OperandToken<Type>*>& eval_stack) {
  auto* op_token_binary = dynamic_cast<OperatorToken<Type, true>*>(token);
  auto* op_token_unary = dynamic_cast<OperatorToken<Type, false>*>(token);
  auto* operand_token = dynamic_cast<OperandToken<Type>*>(token);
  if (operand_token) {
    eval_stack.push(new OperandToken<Type>(operand_token->GetValue()));
  } else if (op_token_binary) {
    ProcessBinary(op_token_binary, eval_stack);
  } else if (op_token_unary) {
    ProcessUnary(op_token_unary, eval_stack);
  } else {
    CleanupStack(eval_stack);
    throw InvalidExpr();
  }
}

template <typename Type>
void Calculator<Type>::ProcessBinary(
    OperatorToken<Type, true>* op_token_binary,
    std::stack<OperandToken<Type>*>& eval_stack) {
  if (eval_stack.size() < 2) {
    CleanupStack(eval_stack);
    throw InvalidExpr();
  }
  OperandToken<Type>* rhs = eval_stack.top();
  eval_stack.pop();
  OperandToken<Type>* lhs = eval_stack.top();
  eval_stack.pop();
  OperandToken<Type>* result = op_token_binary->Calculate(lhs, rhs);
  delete lhs;
  delete rhs;
  eval_stack.push(result);
}

template <typename Type>
void Calculator<Type>::ProcessUnary(
    OperatorToken<Type, false>* op_token_unary,
    std::stack<OperandToken<Type>*>& eval_stack) {
  if (eval_stack.empty()) {
    CleanupStack(eval_stack);
    throw InvalidExpr();
  }

  OperandToken<Type>* operand = eval_stack.top();
  eval_stack.pop();

  OperandToken<Type>* result = op_token_unary->Calculate(operand);
  delete operand;
  eval_stack.push(result);
}

template <typename Type>
void Calculator<Type>::ValidateStackFinalState(
    std::stack<OperandToken<Type>*>& eval_stack) {
  if (eval_stack.size() != 1) {
    CleanupStack(eval_stack);
    throw InvalidExpr();
  }
}

template <typename Type>
Type Calculator<Type>::ExtractFinalResult(
    std::stack<OperandToken<Type>*>& eval_stack) {
  OperandToken<Type>* result_token = eval_stack.top();
  Type result = result_token->GetValue();
  delete result_token;

  return result;
}

template <typename Type>
void Calculator<Type>::CleanupStack(
    std::stack<OperandToken<Type>*>& eval_stack) {
  while (!eval_stack.empty()) {
    delete eval_stack.top();
    eval_stack.pop();
  }
}
