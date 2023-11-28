#include "error.hpp"
#include <string>

ErrorException::ErrorException(std::string message) {
  this->message = message;
}

std::string ErrorException::getMessage() const {
  return message;
}

//----------------------------------------------------------------------------------------

void error(std::string message) {
  throw ErrorException(message);
}