#pragma once
#include <functional>

#include "AbstractToken.hpp"
#include "Exceptions.hpp"
#include "OperandToken.hpp"

template <typename T, bool IsBinary>
class OperatorToken : public AbstractToken {
 public:
  OperatorToken(const std::string& op_symbol);

  OperandToken<T>* Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const;
  OperandToken<T>* Calculate(OperandToken<T>* operand) const;

 private:
  std::function<T(const T&, const T&)> binary_op_;
  std::function<T(const T&)> unary_op_;

  std::function<T(const T&, const T&)> GetBinaryOp(const std::string& op_str);
  std::function<T(const T&)> GetUnaryOp(const std::string& op_str);
};

template <typename T, bool IsBinary>
OperatorToken<T, IsBinary>::OperatorToken(const std::string& op_symbol)
    : AbstractToken(op_symbol) {
  if (IsBinary) {
    binary_op_ = GetBinaryOp(op_symbol);
  } else {
    unary_op_ = GetUnaryOp(op_symbol);
  }
}

template <typename T, bool IsBinary>
OperandToken<T>* OperatorToken<T, IsBinary>::Calculate(
    OperandToken<T>* lhs, OperandToken<T>* rhs) const {
  static_assert(IsBinary, "Cannot call binary Calculate on unary operator");
  if ((lhs == nullptr) || (rhs == nullptr)) {
    throw InvalidExpr();
  }
  T result = binary_op_(lhs->GetValue(), rhs->GetValue());
  return new OperandToken<T>(result);
}

template <typename T, bool IsBinary>
OperandToken<T>* OperatorToken<T, IsBinary>::Calculate(
    OperandToken<T>* operand) const {
  static_assert(!IsBinary, "Cannot call unary Calculate on binary operator");
  if (operand == nullptr) {
    throw InvalidExpr();
  }
  T result = unary_op_(operand->GetValue());
  return new OperandToken<T>(result);
}

template <typename T, bool IsBinary>
std::function<T(const T&, const T&)> OperatorToken<T, IsBinary>::GetBinaryOp(
    const std::string& op_str) {
  if (op_str == "+") {
    return [](const T& lhs, const T& rhs) { return lhs + rhs; };
  }
  if (op_str == "-") {
    return [](const T& lhs, const T& rhs) { return lhs - rhs; };
  }
  if (op_str == "*") {
    return [](const T& lhs, const T& rhs) { return lhs * rhs; };
  }
  if (op_str == "/") {
    return [](const T& lhs, const T& rhs) {
      if (rhs == T(0)) {
        throw InvalidExpr();
      }
      return lhs / rhs;
    };
  }
  throw InvalidExpr();
}

template <typename T, bool IsBinary>
std::function<T(const T&)> OperatorToken<T, IsBinary>::GetUnaryOp(
    const std::string& op_str) {
  if (op_str == "-") {
    return [](const T& operand) { return -operand; };
  }
  if (op_str == "+") {
    return [](const T& operand) { return +operand; };
  }
  throw InvalidExpr();
}
