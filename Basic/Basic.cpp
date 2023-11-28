/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"
#include "Utils/tokenScanner.hpp"
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"

/* Function prototypes */

std::string keyWords[12] = {
    "IF", "LET", "PRINT", "INPUT", "RUN", "LIST", "QUIT", "HELP", "CLEAR", "GOTO", "REM", "END"};

void processLine(std::string line, Program& program, EvalState& state);

bool isKeyWords(const std::string& token);

/* Main program */

int main() {
  // freopen("in.in", "r", stdin);
  EvalState state;
  Program program;
  // cout << "Stub implementation of BASIC" << endl;
  while (true) {
    try {
      std::string input;
      getline(std::cin, input);
      if (input.empty())
        return 0;
      processLine(input, program, state);
    } catch (ErrorException& ex) {
      std::cout << ex.getMessage() << std::endl;
    }
  }
  return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program& program, EvalState& state) {
  if (!isalnum(line[0])) {
    error("SYNTAX ERROR\n");
    return;
  }
  // program line
  if (isdigit(line[0])) {
    int i = 0, title = 0;
    while (isdigit(line[i])) {
      title = title * 10 + line[i++] - '0';
    }

    int endpoint = line.size() - 1;
    while (line[endpoint] == ' ') {
      line.pop_back();
      endpoint--;
    }
    if (line.size() <= i) {
      program.removeSourceLine(title);
      return;
    }
    // if empty or all space, we need to delete

    line = line.substr(i + 1);
    program.addSourceLine(title, line);
    return;
  }

  TokenScanner scanner;
  scanner.ignoreWhitespace();
  scanner.scanNumbers();
  scanner.setInput(line);
  // 上面是原生代码

  Expression* cur = readE(scanner);
  if (cur->getType() != IDENTIFIER) {
    delete cur;
    error("SYNTAX ERROR");
  }

  if (cur->toString() == "PRINT") {
    delete cur;
    cur = parseExp(scanner);
    Statement* stmt = new PrintStat(cur);
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  if (cur->toString() == "LET") {
    delete cur;
    cur = parseExp(scanner);
    if (cur->getType() != COMPOUND || isKeyWords(dynamic_cast<CompoundExp*>(cur)->getLHS()->toString())) {
      delete cur;
      error("SYNTAX ERROR");
    }
    Statement* stmt = new LetStat(dynamic_cast<CompoundExp*>(cur));
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  if (cur->toString() == "INPUT") {
    delete cur;
    cur = parseExp(scanner);
    if (cur->getType() != IDENTIFIER || isKeyWords(cur->toString())) {
      delete cur;
      error("SYNTAX ERROR");
    }
    Statement* stmt = new InputStat(dynamic_cast<IdentifierExp*>(cur));
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  // BasicInterpreters
  if (scanner.hasMoreTokens()) {
    delete cur;
    error("SYNTAX ERROR");
  }

  if (cur->toString() == "RUN") {
    delete cur;
    Statement* stmt = new BasicInterpreters(RUN);
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  if (cur->toString() == "LIST") {
    delete cur;
    Statement* stmt = new BasicInterpreters(LIST);
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  if (cur->toString() == "CLEAR") {
    delete cur;
    Statement* stmt = new BasicInterpreters(CLEAR);
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  if (cur->toString() == "QUIT") {
    delete cur;
    state.Clear();
    program.clear();
    exit(0);
  }

  if (cur->toString() == "HELP") {
    delete cur;
    Statement* stmt = new BasicInterpreters(HELP);
    stmt->execute(state, program);
    delete stmt;
    return;
  }

  delete cur;

  error("SYNTAX ERROR");
}

bool isKeyWords(const std::string& token) {
  for (int i = 0; i < 12; i++) {
    if (token == keyWords[i])
      return true;
  }
  return false;
}
