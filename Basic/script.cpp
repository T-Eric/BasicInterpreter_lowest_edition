#include <cctype>
#include <iostream>
#include <string>
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"
#include "Utils/tokenScanner.hpp"
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"

int main() {
  std::string str;
  TokenScanner scanner;
  scanner.ignoreWhitespace();
  EvalState state;
  Program program;
  int linenum = 0;
  while (linenum != -1) {
    std::cout << "linenum and line:\n";
    std::cin >> linenum;
    std::cin.get();
    if (linenum == -1)
      break;
    std::getline(std::cin, str);
    program.addSourceLine(linenum, str);
    program.listLines();
  }
  Statement* stmt = new BasicInterpreters(RUN);
  stmt->execute(program.reachProgramState(), program);
  delete stmt;
}
