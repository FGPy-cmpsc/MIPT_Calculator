#pragma once
#include <string>

class AbstractToken {
 public:
  AbstractToken(const std::string& token_str) : str_token_(token_str) {}

  virtual ~AbstractToken() = default;

  const std::string& GetStringToken() const { return str_token_; }

 private:
  std::string str_token_;
};
