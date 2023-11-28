/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <sstream>
#include <string>
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"
#include "Utils/tokenScanner.hpp"
#include "evalstate.hpp"
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"

class Program;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {
 public:
  /*
   * Constructor: Statement
   * ----------------------
   * The base class constructor is empty.  Each subclass must provide
   * its own constructor.
   */

  Statement();

  /*
   * Destructor: ~Statement
   * Usage: delete stmt;
   * -------------------
   * The destructor deallocates the storage for this expression.
   * It must be declared virtual to ensure that the correct subclass
   * destructor is called when deleting a statement.
   */

  virtual ~Statement();

  /*
   * Method: execute
   * Usage: stmt->execute(state);
   * ----------------------------
   * This method executes a BASIC statement.  Each of the subclasses
   * defines its own execute method that implements the necessary
   * operations.  As was true for the expression evaluator, this
   * method takes an EvalState object for looking up variables or
   * controlling the operation of the interpreter.
   */

  virtual void execute(EvalState& state, Program& program) = 0;

  // 这里，一次只会存在一个program并且program内外不通
  // 所以我将在有特殊program时传入program.state
};

class LetStat : public Statement {
 public:
  LetStat(CompoundExp* _exp);
  virtual ~LetStat();
  virtual void execute(EvalState& state, Program& program);

 private:
  CompoundExp* exp;
};

class PrintStat : public Statement {
 public:
  PrintStat(Expression* _exp);
  virtual ~PrintStat();
  virtual void execute(EvalState& state, Program& program);

 private:
  Expression* exp;
};

class InputStat : public Statement {
 public:
  InputStat(IdentifierExp* name);
  virtual ~InputStat();
  virtual void execute(EvalState& state, Program& program);

 private:
  IdentifierExp* name;
};

// in_program stats

class IfThenStat : public Statement {
 public:
  IfThenStat(Expression* _lhs, char _opera, Expression* _rhs, int _thenLine);
  virtual ~IfThenStat();
  virtual void execute(EvalState& state, Program& program);

 private:
  Expression* lhs;
  char opera;
  Expression* rhs;
  int thenLine;
};

class GotoStat : public Statement {
 public:
  GotoStat(int _gotoLine);
  virtual ~GotoStat();
  virtual void execute(EvalState& state, Program& program);

 private:
  int gotoLine;
};

class EndStat : public Statement {
 public:
  EndStat();
  virtual ~EndStat();
  virtual void execute(EvalState& state, Program& program);

 private:
};

class RemStat : public Statement {
 public:
  RemStat();
  virtual ~RemStat();
  virtual void execute(EvalState& state, Program& program);

 private:
};

enum InterpreterType {
  RUN,
  LIST,
  CLEAR,
  QUIT,
  HELP
};

class BasicInterpreters : public Statement {
 public:
  BasicInterpreters(InterpreterType _type);
  virtual ~BasicInterpreters();
  virtual void execute(EvalState& state, Program& program);
  InterpreterType getType();
  void run(EvalState& state, Program& program);
  void list(Program& program);
  void clear(EvalState& state, Program& program);
  void quit(EvalState& state, Program& program);
  void help();

 private:
  InterpreterType type;
};

/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

#endif
