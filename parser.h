#ifndef PARSER_H
#define PARSER_H

#include <map>

class ExprAST;
class PrototypeAST;
class FunctionAST;

extern int curTok;
extern std::map<char, int> BinopPrecedence;

ExprAST *ParsePrimary();
ExprAST *ParseBinOpRHS(int, ExprAST *);
ExprAST *ParseNumberExpr();
ExprAST *ParseExpression();
ExprAST *ParseParenExpr();
ExprAST *ParseIdentifierExpr();
PrototypeAST *ParseExtern();
FunctionAST *ParseDefinition();
FunctionAST *ParseTopLevelExpr();
int getTokPrecedence();
int getNextToken();
ExprAST *Error(const char *Str);
PrototypeAST *ErrorP(const char *Str);
FunctionAST *ErrorF(const char *Str);

#endif
