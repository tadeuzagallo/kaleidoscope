#ifndef AST_H
#define AST_H

#include <string>
#include <vector>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

extern llvm::Module *module;
extern llvm::IRBuilder<> builder;
extern std::map<std::string, llvm::Value*> table;

llvm::Value *ErrorV(const char *Str);

class ExprAST {
public:
  virtual ~ExprAST() {};
  virtual llvm::Value *Codegen();
};

class NumberExprAST : public ExprAST {
  double value;
public:
  NumberExprAST(double v) : value(v) {};
  virtual llvm::Value *Codegen();
};

class VariableExprAST : public ExprAST {
  std::string name;
public:
  VariableExprAST(const std::string &n) : name(n) {};
  virtual llvm::Value *Codegen();
};

class BinaryExprAST : public ExprAST {
  char op;
  ExprAST *lhs, *rhs;
public:
  BinaryExprAST(char o, ExprAST *l, ExprAST *r) : op(o), lhs(l), rhs(r) {};
  virtual llvm::Value *Codegen();
};

class CallExprAST : public ExprAST {
  std::string callee;
  std::vector<ExprAST *> args;
public:
  CallExprAST(const std::string &c, std::vector<ExprAST *> &a) : callee(c), args(a) {};
  virtual llvm::Value *Codegen();
};

class PrototypeAST {
  std::string name;
  std::vector<std::string> args;
public:
  PrototypeAST(const std::string &n, const std::vector<std::string> a) : name(n), args(a) {};
  llvm::Function *Codegen();
};

class FunctionAST {
  PrototypeAST *proto;
  ExprAST *body;
public:
  FunctionAST (PrototypeAST *p, ExprAST *b) : proto(p), body(b) {};
  llvm::Function *Codegen();
};

#endif
