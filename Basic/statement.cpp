/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"

/* Implementation of the Statement class */
/* we assume that all the expressions in the statements are right in syntax */

Statement::Statement() = default;

Statement::~Statement() = default;

// 血的教训：千万，千万不要在成员变量有指针时把析构函数设成default！原理未知，貌似是因为变量的析构函数没有被调用。

LetStat::LetStat(CompoundExp* _exp) {
  exp = _exp;
}
LetStat::~LetStat() {
  delete exp;
  exp = nullptr;
}

void LetStat::execute(EvalState& state, Program& program) {
  if (exp->getLHS()->getType() != IDENTIFIER) {
    std::cout << "SYNTAX ERROR\n";
    return;
  }
  try {
    exp->eval(state);
  } catch (ErrorException& ex) {
    std::cout << ex.getMessage() << std::endl;
  }
}

PrintStat::PrintStat(Expression* _exp) {
  exp = _exp;
}
PrintStat::~PrintStat() {
  delete exp;
  exp = nullptr;
}

void PrintStat::execute(EvalState& state, Program& program) {
  if (exp->getType() == IDENTIFIER && !state.isDefined(exp->toString())) {
    std::cout << "VARIABLE NOT DEFINED\n";
    return;
  }
  try {
    std::cout << exp->eval(state) << '\n';  // 可能需要修改
  } catch (ErrorException& ex) {
    std::cout << ex.getMessage() << std::endl;
  }
}

InputStat::InputStat(IdentifierExp* _name) {
  name = _name;
}
InputStat::~InputStat() {
  delete name;
  name = nullptr;
}

void InputStat::execute(EvalState& state, Program& program) {
  int cur, neg = 1;
  bool valid = true;
  std::string input;
  while (true) {
    std::cout << " ? ";
    getline(std::cin, input);
    // std::cin.get();
    cur = 0;
    valid = true;
    if (input.size() == 0) {
      std::cout << "INVALID NUMBER\n";
      valid = false;
      continue;
    }
    if (input[0] == '-') {
      if (input.size() == 1) {
        std::cout << "INVALID NUMBER\n";
        valid = false;
        continue;
      }
      neg = -1;
      input.erase(input.begin());
    }
    for (int i = 0; i < input.size(); i++) {
      if (!isdigit(input[i])) {
        std::cout << "INVALID NUMBER\n";  // 用error还能保证继续吗？
        valid = false;
        break;
      }
      cur = cur * 10 + input[i] - '0';
    }
    if (!valid)
      continue;
    state.setValue(name->toString(), cur * neg);
    return;
  }
}

IfThenStat::IfThenStat(Expression* _lhs, char _opera, Expression* _rhs, int _thenLine) {
  lhs = _lhs;
  opera = _opera;
  rhs = _rhs;
  thenLine = _thenLine;
}
IfThenStat::~IfThenStat() {
  delete lhs;
  delete rhs;
  lhs = nullptr;
  rhs = nullptr;
}

void IfThenStat::execute(EvalState& state, Program& program) {
  if (!program.isInLine(thenLine)) {
    std::cout << "LINENUMBER ERROR";
    program.runProgram(state, -1);  // endit
  }
  switch (opera) {
    case '=':
      if (lhs->eval(state) == rhs->eval(state)) {
        program.runProgram(state, thenLine);
      }
      break;
    case '>':
      if (lhs->eval(state) > rhs->eval(state)) {
        program.runProgram(state, thenLine);
      }
      break;
    case '<':
      if (lhs->eval(state) < rhs->eval(state)) {
        program.runProgram(state, thenLine);
      }
      break;
  }
}

GotoStat::GotoStat(int _gotoline) {
  gotoLine = _gotoline;
}
GotoStat::~GotoStat() = default;

void GotoStat::execute(EvalState& state, Program& program) {
  if (!program.isInLine(gotoLine)) {
    std::cout << "LINE NUMBER ERROR\n";
    program.runProgram(state, -1);  // 直接abort
  }
  if (program.isInLine(gotoLine)) {
    program.runProgram(state, gotoLine);  // 只要能END就行了嘛
  } else {
    program.runProgram(state, program.getNextLineNumber(gotoLine));
  }
}

EndStat::EndStat() = default;
EndStat::~EndStat() = default;

void EndStat::execute(EvalState& state, Program& program) {
  program.endProgram();
  program.runProgram(state, -1);
}

RemStat::RemStat() = default;
RemStat::~RemStat() = default;
void RemStat::execute(EvalState& state, Program& program) {
  return;
}

BasicInterpreters::BasicInterpreters(InterpreterType _type) {
  type = _type;
}
BasicInterpreters::~BasicInterpreters() = default;

void BasicInterpreters::execute(EvalState& state, Program& program) {
  switch (type) {
    case (RUN):
      run(state, program);
      break;
    case (LIST):
      list(program);
      break;
    case (CLEAR):
      clear(state, program);
      break;
    case (QUIT):
      quit(state, program);
      break;
    case (HELP):
      help();
      break;
  }
}

void BasicInterpreters::run(EvalState& state, Program& program) {
  program.startProgram();
  program.runProgram(state, program.getFirstLineNumber());
}

void BasicInterpreters::list(Program& program) {
  program.listLines();
}

void BasicInterpreters::clear(EvalState& state, Program& program) {
  program.clear();
  state.Clear();
}

void BasicInterpreters::quit(EvalState& state, Program& program) {
  state.Clear();
  program.clear();
  exit(0);
}

void BasicInterpreters::help() {
  std::cout << "Look at this: https://github.com/ACMClassCourse-2023/Basic-Interpreter-2023\n";
}

// todo
