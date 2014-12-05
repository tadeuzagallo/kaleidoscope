#include "ast.h"
#include "parser.h"
#include "lexer.h"

static void HandleDefinition() {
  if (FunctionAST *f = ParseDefinition()) {
    if (llvm::Function *fn = f->Codegen()) {
      fprintf(stderr, "Parsed a function definition.\n");
      fn->dump();
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern() {
  if (PrototypeAST *proto = ParseExtern()) {
    if (llvm::Function *fn = proto->Codegen()) {
      fprintf(stderr, "Parsed an extern\n");
      fn->dump();
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (FunctionAST *f = ParseTopLevelExpr()) {
    if (llvm::Function *fn = f->Codegen()) {
      fprintf(stderr, "Parsed a top-level expr\n");
      fn->dump();
    }
  } else {
    getNextToken();
  }
}

static void mainLoop() {
  while(1) {
    fprintf(stderr, "ready> ");
    switch (curTok) {
      case tok_eof: return;
      case ';': getNextToken(); break;
      case tok_def: HandleDefinition(); break;
      case tok_extern: HandleExtern(); break;
      default: HandleTopLevelExpression(); break;
    }
  }
}

extern "C" double putchard(double c) {
  putchar((char)c);
  return 0;
}

int main() {
  BinopPrecedence['<'] = 10;
  BinopPrecedence['+'] = 20;
  BinopPrecedence['-'] = 20;
  BinopPrecedence['*'] = 40;

  // Prime the first token.
  fprintf(stderr, "ready> ");
  getNextToken();

  module = new llvm::Module("My cool JIT", llvm::getGlobalContext());
  // Run the main "interpreter loop" now.
  mainLoop();

  module->dump();

  return 0;
}

