#pragma once
#include <sstream>

#include "AbstractToken.hpp"
#include "Exceptions.hpp"

template <typename T>
class OperandToken : public AbstractToken {
 public:
  OperandToken(const std::string& token_str);

  OperandToken(const T& val);

  const T& GetValue() const;

 private:
  T value_;
};

template <typename T>
OperandToken<T>::OperandToken(const std::string& token_str)
    : AbstractToken(token_str) {
  std::istringstream iss(token_str);
  if (!(iss >> value_)) {
    throw InvalidExpr();
  }
}

template <typename T>
OperandToken<T>::OperandToken(const T& val)
    : AbstractToken(std::to_string(val)), value_(val) {}

template <typename T>
const T& OperandToken<T>::GetValue() const {
  return value_;
}
