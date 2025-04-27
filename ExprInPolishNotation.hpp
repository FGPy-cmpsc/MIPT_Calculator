#pragma once
#include <algorithm>
#include <cctype>
#include <stack>
#include <string>
#include <vector>

#include "AbstractToken.hpp"
#include "Exceptions.hpp"
#include "OperandToken.hpp"
#include "OperatorToken.hpp"

template <typename T>
class ExprInPolishNotation {
 public:
  ExprInPolishNotation(const std::string& expr) { ParseExpression(expr); }
  ~ExprInPolishNotation();
  const std::vector<AbstractToken*>& GetTokens() const { return tokens_; }

 private:
  std::vector<AbstractToken*> tokens_;
  const int kUnarOper = 3;

  void ParseExpression(const std::string& expr);
  int GetPrecedence(const std::string& oper);
  bool IsOperator(const std::string& token);
  bool IsOperand(const std::string& token);
  std::vector<AbstractToken*> InfixToPostfix(
      const std::vector<std::string>& tokens_str);
  void ProcessTokens(const std::vector<std::string>& tokens_str,
                     std::vector<AbstractToken*>& output,
                     std::stack<std::string>& op_stack);
  void ProcessClosingParenthesis(std::vector<AbstractToken*>& output,
                                 std::stack<std::string>& op_stack);
  void ProcessOperator(const std::string& token_str,
                       std::vector<AbstractToken*>& output,
                       std::stack<std::string>& op_stack);
  void ProcessRemainingOperators(std::vector<AbstractToken*>& output,
                                 std::stack<std::string>& op_stack);
  void CleanupTokens(std::vector<AbstractToken*>& tokens_vec);
  std::vector<std::string> Tokenize(const std::string& expr);
  std::vector<std::string> HandleUnaryOperators(
      const std::vector<std::string>& tokens_str);
  AbstractToken* CreateOperatorToken(const std::string& op_str);
};

template <typename T>
ExprInPolishNotation<T>::~ExprInPolishNotation() {
  for (auto* token : tokens_) {
    delete token;
  }
}

template <typename T>
void ExprInPolishNotation<T>::ParseExpression(const std::string& expr) {
  std::vector<std::string> tokens_str = Tokenize(expr);
  tokens_str = HandleUnaryOperators(tokens_str);
  std::vector<AbstractToken*> postfix_tokens = InfixToPostfix(tokens_str);
  tokens_ = postfix_tokens;
}

template <typename T>
int ExprInPolishNotation<T>::GetPrecedence(const std::string& oper) {
  if (oper == "+" || oper == "-") {
    return 1;
  }
  if (oper == "*" || oper == "/") {
    return 2;
  }
  if (oper == "~" || oper == "@") {
    return kUnarOper;
  }
  return 0;
}

template <typename T>
bool ExprInPolishNotation<T>::IsOperator(const std::string& token) {
  return token == "+" || token == "-" || token == "*" || token == "/" ||
         token == "~" || token == "@";
}

template <typename T>
bool ExprInPolishNotation<T>::IsOperand(const std::string& token) {
  return !IsOperator(token) && token != "(" && token != ")";
}

template <typename T>
std::vector<AbstractToken*> ExprInPolishNotation<T>::InfixToPostfix(
    const std::vector<std::string>& tokens_str) {
  std::vector<AbstractToken*> output;
  std::stack<std::string> op_stack;

  ProcessTokens(tokens_str, output, op_stack);
  ProcessRemainingOperators(output, op_stack);

  return output;
}

template <typename T>
void ExprInPolishNotation<T>::ProcessTokens(
    const std::vector<std::string>& tokens_str,
    std::vector<AbstractToken*>& output, std::stack<std::string>& op_stack) {
  for (const std::string& token_str : tokens_str) {
    if (IsOperand(token_str)) {
      output.push_back(new OperandToken<T>(token_str));
    } else if (token_str == "(") {
      op_stack.push(token_str);
    } else if (token_str == ")") {
      ProcessClosingParenthesis(output, op_stack);
    } else if (IsOperator(token_str)) {
      ProcessOperator(token_str, output, op_stack);
    } else {
      CleanupTokens(output);
      throw InvalidExpr();
    }
  }
}

template <typename T>
void ExprInPolishNotation<T>::ProcessClosingParenthesis(
    std::vector<AbstractToken*>& output, std::stack<std::string>& op_stack) {
  while (!op_stack.empty() && op_stack.top() != "(") {
    output.push_back(CreateOperatorToken(op_stack.top()));
    op_stack.pop();
  }

  if (!op_stack.empty() && op_stack.top() == "(") {
    op_stack.pop();
  } else {
    CleanupTokens(output);
    throw InvalidExpr();
  }
}

template <typename T>
void ExprInPolishNotation<T>::ProcessOperator(
    const std::string& token_str, std::vector<AbstractToken*>& output,
    std::stack<std::string>& op_stack) {
  while (!op_stack.empty() &&
         GetPrecedence(op_stack.top()) >= GetPrecedence(token_str)) {
    output.push_back(CreateOperatorToken(op_stack.top()));
    op_stack.pop();
  }
  op_stack.push(token_str);
}

template <typename T>
void ExprInPolishNotation<T>::ProcessRemainingOperators(
    std::vector<AbstractToken*>& output, std::stack<std::string>& op_stack) {
  while (!op_stack.empty()) {
    if (op_stack.top() == "(" || op_stack.top() == ")") {
      CleanupTokens(output);
      throw InvalidExpr();
    }
    output.push_back(CreateOperatorToken(op_stack.top()));
    op_stack.pop();
  }
}

template <typename T>
void ExprInPolishNotation<T>::CleanupTokens(
    std::vector<AbstractToken*>& tokens_vec) {
  for (auto* token : tokens_vec) {
    delete token;
  }
  tokens_vec.clear();
}

template <typename T>
std::vector<std::string> ExprInPolishNotation<T>::Tokenize(
    const std::string& expr) {
  std::vector<std::string> tokens;
  size_t i = 0;
  while (i < expr.length()) {
    if (std::isspace(expr[i]) != 0) {
      ++i;
      continue;
    }
    if ((std::isdigit(expr[i]) != 0) || expr[i] == '.') {
      size_t start = i;
      while (i < expr.length() &&
             (std::isdigit(expr[i]) != 0 || expr[i] == '.')) {
        ++i;
      }
      tokens.push_back(expr.substr(start, i - start));
    } else if (std::isalpha(expr[i]) != 0) {
      throw InvalidExpr();
    } else {
      tokens.push_back(std::string(1, expr[i]));
      ++i;
    }
  }
  return tokens;
}

template <typename T>
std::vector<std::string> ExprInPolishNotation<T>::HandleUnaryOperators(
    const std::vector<std::string>& tokens_str) {
  std::vector<std::string> result;
  for (size_t i = 0; i < tokens_str.size(); ++i) {
    if (tokens_str[i] == "-") {
      if (i == 0 || tokens_str[i - 1] == "(" || IsOperator(tokens_str[i - 1])) {
        result.push_back("~");
      } else {
        result.push_back(tokens_str[i]);
      }
    } else if (tokens_str[i] == "+") {
      if (i == 0 || tokens_str[i - 1] == "(" || IsOperator(tokens_str[i - 1])) {
        result.push_back("@");
      } else {
        result.push_back(tokens_str[i]);
      }
    } else {
      result.push_back(tokens_str[i]);
    }
  }
  return result;
}

template <typename T>
AbstractToken* ExprInPolishNotation<T>::CreateOperatorToken(
    const std::string& op_str) {
  if (op_str == "~") {
    return new OperatorToken<T, false>("-");
  }
  if (op_str == "@") {
    return new OperatorToken<T, false>("+");
  }
  if (op_str == "+" || op_str == "-" || op_str == "*" || op_str == "/") {
    return new OperatorToken<T, true>(op_str);
  }
  throw InvalidExpr();
}
