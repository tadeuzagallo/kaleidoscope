#include "parser.h"
#include "lexer.h"
#include "ast.h"

#include <vector>

int curTok = 0;
std::map<char, int> BinopPrecedence = std::map<char, int>();

int getNextToken() {
  return curTok = gettok();
}

ExprAST *Error(const char *Str) { fprintf(stderr, "Error: %s\n", Str);return 0;}
PrototypeAST *ErrorP(const char *Str) { Error(Str); return 0; }
FunctionAST *ErrorF(const char *Str) { Error(Str); return 0; }

ExprAST *ParsePrimary() {
  switch (curTok) {
    case tok_identifier: return ParseIdentifierExpr();
    case tok_number: return ParseNumberExpr();
    case '(': return ParseParenExpr();
    default: return Error("Unknown token when expecting an expression");
  }
}

ExprAST *ParseBinOpRHS(int prec, ExprAST *lhs) {
  while (1) {
    int tokPrec = getTokPrecedence();

    if (tokPrec < prec) {
      return lhs;
    }

    int binOp = curTok;
    getNextToken();

    ExprAST *rhs = ParsePrimary();

    if (!rhs) {
      return 0;
    }

    int nextPrec = getTokPrecedence();
    if (tokPrec < nextPrec) {
      rhs = ParseBinOpRHS(tokPrec + 1, rhs);
      if (!rhs) {
        return 0;
      }
    }

    lhs = new BinaryExprAST(binOp, lhs, rhs);
  }
}

ExprAST *ParseExpression() {
  ExprAST *lhs = ParsePrimary();
  if (!lhs) {
    return 0;
  }

  return ParseBinOpRHS(0, lhs);
}

ExprAST *ParseNumberExpr() {
  ExprAST *result = new NumberExprAST(NumVal);
  getNextToken();
  return result;
}

ExprAST *ParseParenExpr() {
  getNextToken();
  ExprAST *V = ParseExpression();

  if (!V) {
    return 0;
  }
  
  if (curTok != ')') {
    return Error("Expected ')'");
  }

  getNextToken();
  return V;
}

ExprAST *ParseIdentifierExpr() {
  std::string name = IdentifierStr;

  getNextToken();

  if (curTok != '(') {
    return new VariableExprAST(name);
  }

  getNextToken();
  
  std::vector<ExprAST *> args;
  if (curTok != ')') {
    while (1) {
      ExprAST *arg = ParseExpression();
      if (!arg) return 0;
      args.push_back(arg);

      if (curTok == ')') {
        break;
      }

      if (curTok != ',') {
        return Error("Expected ')' or ',' in argument list");
      }

      getNextToken();
    }
  }

  getNextToken();

  return new CallExprAST(name, args);
}


int getTokPrecedence() {
  if (!isascii(curTok)) {
    return -1;
  }

  int tokPrec = BinopPrecedence[curTok];

  if (tokPrec <= 0) {
    return -1;
  }

  return tokPrec;
}

PrototypeAST *ParsePrototype() {
  if (curTok != tok_identifier) {
    return ErrorP("Expected function name in prototype");
  }

  std::string name = IdentifierStr;
  getNextToken();

  if (curTok != '(') {
    return ErrorP("Expected '(' in prototype");
  }

  std::vector<std::string> argNames;
  while (getNextToken() == tok_identifier) {
    argNames.push_back(IdentifierStr);
  }

  if (curTok != ')') {
    return ErrorP("Expected ')' in prototype");
  }

  getNextToken();

  return new PrototypeAST(name, argNames);
}

FunctionAST *ParseDefinition() {
  getNextToken();

  PrototypeAST *proto = ParsePrototype();
  if (!proto) {
    return 0;
  }

  if (ExprAST *E = ParseExpression()) {
    return new FunctionAST(proto, E);
  }

  return 0;
}

PrototypeAST *ParseExtern() {
  getNextToken();
  return ParsePrototype();
}

FunctionAST *ParseTopLevelExpr() {
  if (ExprAST *e = ParseExpression()) {
    PrototypeAST *proto = new PrototypeAST("", std::vector<std::string>());
    return new FunctionAST(proto, e);
  }

  return 0;
}
