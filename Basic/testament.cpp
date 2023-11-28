#include <cctype>
#include <iostream>
#include <string>
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"
#include "Utils/tokenScanner.hpp"
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"

int main(int argc, const char** argv) {
  std::cout << "test\n";
  int lineNumber;
  std::string str;
  EvalState state;
  Program program;
  Expression* exp;
  int i = 0;
  while (++i < 10) {
    std::cout << "line Number and line:\n";
    std::cin >> lineNumber;
    std::cin.get();
    std::getline(std::cin, str);
    program.addSourceLine(lineNumber, str);
    std::cout << "try a list:\n";
    program.listLines();
    program.removeSourceLine(lineNumber);
    std::cout << program.getSourceLine(lineNumber) << '\n';
  }
  delete exp;
  return 0;
}