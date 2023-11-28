/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"

// THERE'S ONE AND ONLY ONE PROGRAM AT THE SAME TIME

Program::Program() = default;

Program::~Program() = default;

void Program::clear() {
  lineList.clear();
  programTable.clear();
  std::unordered_map<int, Statement*>::iterator mapIt;
  for (mapIt = lineTable.begin(); mapIt != lineTable.end(); mapIt++) {
    delete mapIt->second;
    mapIt->second = nullptr;
  }
  lineTable.clear();
}  // ok

void Program::addSourceLine(int lineNumber, const std::string& line) {
  std::set<int>::iterator setIt = lineList.find(lineNumber);
  Statement* stmt = stringToStmt(line);
  if (stmt == nullptr) {
    return;
  }
  if (setIt != lineList.end()) {
    lineList.erase(lineNumber);
    programTable.erase(lineNumber);
    delete lineTable[lineNumber];
    lineTable[lineNumber] = nullptr;
    lineTable.erase(lineNumber);
  }
  lineList.insert(lineNumber);
  programTable.insert(std::make_pair(lineNumber, line));
  lineTable.insert(std::make_pair(lineNumber, stmt));
}

void Program::removeSourceLine(int lineNumber) {
  std::set<int>::iterator setIt = lineList.find(lineNumber);
  if (setIt == lineList.end())
    return;
  lineList.erase(setIt);
  programTable.erase(lineNumber);
  delete lineTable[lineNumber];
  lineTable[lineNumber] = nullptr;
  lineTable.erase(lineNumber);
  return;
}  // ok

std::string Program::getSourceLine(int lineNumber) {
  std::unordered_map<int, std::string>::iterator mapIt = programTable.find(lineNumber);
  if (mapIt == programTable.end())
    return "";
  return mapIt->second;
}

void Program::setParsedStatement(int lineNumber, Statement* stmt) {
  // std::set<int>::iterator setIt = lineList.find(lineNumber);
  // if (stmt == nullptr)
  //   return;
  // if (setIt != lineList.end()) {
  //   lineList.erase(lineNumber);
  //   programTable.erase(lineNumber);
  //   lineTable.erase(lineNumber);
  // }
  // lineList.insert(lineNumber);
  // programTable.insert(std::make_pair(lineNumber, line));
  // lineTable.insert(std::make_pair(lineNumber, stmt));
}  //?

// 将一行string转化为statement

Statement* Program::getParsedStatement(int lineNumber) {
  std::unordered_map<int, Statement*>::iterator mapIt = lineTable.find(lineNumber);
  if (mapIt == lineTable.end())
    return nullptr;
  return mapIt->second;
}

int Program::getFirstLineNumber() {
  if (programTable.empty())
    return -1;
  return *lineList.begin();
}

/*like the upper_bound*/

int Program::getNextLineNumber(int lineNumber) {
  std::set<int>::iterator setIt = lineList.upper_bound(lineNumber);
  if (setIt == lineList.end())
    return -1;
  return *setIt;
}

int Program::getLastLineNumber() {
  if (programTable.empty())
    return -1;
  return *lineList.rbegin();
}

bool Program::isInLine(int lineNumber) {
  return lineTable.find(lineNumber) != lineTable.end();
}

void Program::listLines() {
  std::set<int>::iterator setIt;
  for (setIt = lineList.begin(); setIt != lineList.end(); setIt++)
    std::cout << *setIt << ' ' << programTable[*setIt] << '\n';
}

Statement* Program::stringToStmt(const std::string& str) {
  if (str.empty())
    return nullptr;
  bool isempty = true;
  for (int i = 0; i < str.size(); i++) {
    if (str[i] != ' ') {
      isempty = false;
      break;
    }
  }
  if (isempty)
    return nullptr;

  TokenScanner scanner(str);
  scanner.ignoreWhitespace();
  Expression* cur = readE(scanner);

  if (cur->getType() != IDENTIFIER) {
    std::cout << "SYNTAX ERROR\n";
    delete cur;
    cur = nullptr;
    return nullptr;
  }

  if (cur->toString() == "LET") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    delete cur;
    cur = readE(scanner);
    if (cur->getType() != COMPOUND || scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    if (isKeyWords(dynamic_cast<CompoundExp*>(cur)->getLHS()->toString())) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    return new LetStat(dynamic_cast<CompoundExp*>(cur));
  }

  if (cur->toString() == "INPUT") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    delete cur;
    cur = readE(scanner);
    if (cur->getType() != IDENTIFIER || scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    if (isKeyWords(cur->toString())) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    return new InputStat(dynamic_cast<IdentifierExp*>(cur));
  }

  if (cur->toString() == "PRINT") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    scanner.ignoreWhitespace();
    delete cur;
    cur = readE(scanner);
    if (scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    return new PrintStat(cur);
  }

  if (cur->toString() == "IF") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    // 直接拆解原式
    int pointer = 3;
    bool isValid = false;
    char opera = '=';
    for (; pointer < str.size(); pointer++) {
      if (str[pointer] == '=') {
        isValid = true;
        break;
      } else if (str[pointer] == '<') {
        isValid = true;
        opera = '<';
        break;
      } else if (str[pointer] == '>') {
        isValid = true;
        opera = '>';
        break;
      }
    }
    if (!isValid) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }

    std::string LHS = str.substr(3, pointer - 3);
    scanner.setInput(LHS);
    Expression* lhs = readE(scanner);
    std::string rightSide;
    if (str[pointer + 1] == ' ')
      rightSide = str.substr(pointer + 2);
    else
      rightSide = str.substr(pointer + 1);
    scanner.setInput(rightSide);
    Expression* rhs = readE(scanner);
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    // then
    delete cur;
    cur = readE(scanner);
    if (cur->toString() != "THEN") {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    // thenline
    delete cur;
    cur = parseExp(scanner);
    if (cur->getType() != CONSTANT) {
      delete cur;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    int thenLine = dynamic_cast<ConstantExp*>(cur)->getValue();
    delete cur;
    cur = nullptr;
    if (thenLine < 0) {
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    return new IfThenStat(lhs, opera, rhs, thenLine);
  }

  if (cur->toString() == "GOTO") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    delete cur;
    cur = readE(scanner);
    if (cur->getType() != CONSTANT) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    int gotoLine = dynamic_cast<ConstantExp*>(cur)->getValue();
    delete cur;
    cur = nullptr;
    return new GotoStat(gotoLine);
  }

  if (cur->toString() == "END") {
    if (scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    delete cur;
    cur = nullptr;
    return new EndStat;
  }

  if (cur->toString() == "REM") {
    if (!scanner.hasMoreTokens()) {
      delete cur;
      cur = nullptr;
      std::cout << "SYNTAX ERROR\n";
      return nullptr;
    }
    delete cur;
    cur = nullptr;
    return new RemStat;
  }

  delete cur;
  cur = nullptr;
  std::cout << "SYNTAX ERROR\n";
  return nullptr;
}

void Program::startProgram() {
  abort = false;
}

void Program::endProgram() {
  abort = true;
}

void Program::runProgram(EvalState& state, int lineNumber) {
  if (lineNumber == -1) {
    endProgram();
    return;
  }
  if (!abort) {
    Statement* stmt = lineTable[lineNumber];
    stmt->execute(state, *this);
    if (!abort) {
      int nextLine = getNextLineNumber(lineNumber);
      runProgram(state, nextLine);
    }
  }
}

std::string keywords[] = {
    "IF", "LET", "PRINT", "INPUT", "RUN", "LIST", "QUIT", "HELP", "CLEAR", "GOTO", "REM", "END"};

bool Program::isKeyWords(const std::string& token) {
  for (int i = 0; i < keywords->size(); i++) {
    if (token == keywords[i])
      return true;
  }
  return false;
}

// more func to add
// todo
