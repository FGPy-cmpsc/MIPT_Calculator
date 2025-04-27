#pragma once
#include "AbstractToken.hpp"
#include "Exceptions.hpp"
#include "ExprInPolishNotation.hpp"

class BracketToken : public AbstractToken {
 public:
  BracketToken(const std::string& token_str);
  bool IsOpening() const { return GetStringToken() == "("; }
};

BracketToken::BracketToken(const std::string& token_str)
    : AbstractToken(token_str) {
  if (token_str != "(" && token_str != ")") {
    throw InvalidExpr();
  }
}
