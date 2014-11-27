#include <string>
#include <vector>

class ExprAST {
public:
  virtual ~ExprAST() {};
};

class NumberExprAST : public ExprAST {
  double value;
public:
  NumberExprAST(double v) : value(v) {};
};

class VariableExprAST : public ExprAST {
  std::string name;
public:
  VariableExprAST(const std::string &n) : name(n) {};
};

class BinaryExprAST : public ExprAST {
  char op;
  ExprAST *lhs, *rhs;
public:
  BinaryExprAST(char o, ExprAST *l, ExprAST *r) : op(o), lhs(l), rhs(r) {};
};

class CallExprAST : public ExprAST {
  std::string callee;
  std::vector<ExprAST *> args;
public:
  CallExprAST(const std::string &c, std::vector<ExprAST *> &a) : callee(c), args(a) {};
};

class PrototypeAST {
  std::string name;
  std::vector<std::string> args;
public:
  PrototypeAST(const std::string &n, const std::vector<std::string> a) : name(n), args(a) {};
};

class FunctionAST {
  PrototypeAST *proto;
  ExprAST *body;
public:
  FunctionAST (PrototypeAST *p, ExprAST *b) : proto(p), body(b) {};
};
