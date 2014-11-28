#include "ast.h"
#include "parser.h"

llvm::Module *module;
llvm::IRBuilder<> builder = llvm::IRBuilder<>(llvm::getGlobalContext());
std::map<std::string, llvm::Value *> table = std::map<std::string, llvm::Value *>();

llvm::Value *ErrorV(const char *Str) {
  Error(Str);
  return 0;
}

llvm::Value *NumberExprAST::Codegen() {
  return llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(value));
}

llvm::Value *VariableExprAST::Codegen() {
  llvm::Value *v = table[name];
  return v ?: ErrorV("Unknown variable name");
}

llvm::Value *BinaryExprAST::Codegen() {
  llvm::Value *l = lhs->Codegen();
  llvm::Value *r = rhs->Codegen();

  switch (op) {
    case '+': return builder.CreateFAdd(l, r, "addtmp");
    case '-': return builder.CreateFSub(l, r, "subtmp");
    case '*': return builder.CreateFMul(l, r, "multmp");
    case '<':
      l = builder.CreateFCmpULT(l, r, "cmptmp");
      return builder.CreateUIToFP(l, llvm::Type::getDoubleTy(llvm::getGlobalContext()), "booltmp");
    default: return ErrorV("Invalid binary operator");
  }
}

llvm::Value *CallExprAST::Codegen() {
  llvm::Function *fn = module->getFunction(callee);

  if (!fn) {
    return ErrorV("Unknown function referenced");
  }

  if (fn->arg_size() != args.size()) {
    return ErrorV("Incorrect # arguments passed");
  }

  std::vector<llvm::Value *> argsV;
  for (unsigned int i = 0, e = args.size(); i != e; ++i) {
    argsV.push_back(args[i]->Codegen());
    if (!argsV.back()) return 0;
  }

  return builder.CreateCall(fn, argsV, "calltmp");
}

llvm::Function *PrototypeAST::Codegen() {
  std::vector<llvm::Type *> doubles (args.size(),
      llvm::Type::getDoubleTy(llvm::getGlobalContext()));

  llvm::FunctionType *ft = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(llvm::getGlobalContext()),
      doubles, false);

  llvm::Function *fn = llvm::Function::Create(ft,
      llvm::Function::ExternalLinkage,
      name,
      module);

  if (fn->getName() != name) {
    fn->eraseFromParent();
    fn = module->getFunction(name);
  }

  if (!fn->empty()) {
    ErrorF("Redefinition of function");
    return 0;
  }

  if (fn->arg_size() != args.size()) {
    ErrorF("Redefinition of function with different # arguments");
    return 0;
  }

  unsigned idx = 0;
  for (llvm::Function::arg_iterator ai = fn->arg_begin(); idx != args.size(); ++ai, ++idx) {
    ai->setName(args[idx]);
    table[args[idx]] = ai;
  }

  return fn;
}

llvm::Function *FunctionAST::Codegen() {
  table.clear();

  llvm::Function *fn = proto->Codegen();
  if (!fn) {
    return 0;
  }

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", fn);
  builder.SetInsertPoint(bb);

  if (llvm::Value *retVal = body->Codegen()) {
    builder.CreateRet(retVal);
    llvm::verifyFunction(*fn);
    return fn;
  }

  if (!fn->empty()) {
    fn->eraseFromParent();
  }
  return 0;
}
