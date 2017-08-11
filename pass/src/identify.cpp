#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "identify.h"

using namespace llvm;

static const char* EnabledName = "easy_jit_enabled";

FunToInlineMap GetFunctionsToJit(llvm::Module &M) {
  FunToInlineMap Map;

  // nothing marked
  Function* EasyJitEnabled = M.getFunction(EnabledName);
  if(!EasyJitEnabled)
    return Map;

  //collect the functions where the function is used
  while(!EasyJitEnabled->user_empty()) {
    User* U = EasyJitEnabled->user_back();
    CallInst* AsCall = dyn_cast<CallInst>(U);

    assert(AsCall && AsCall->getCalledFunction() == EasyJitEnabled
           && "Easy jit function not used as a function call!");

    Values Args(AsCall->arg_begin(), AsCall->arg_end());
    Function* ToJit = AsCall->getParent()->getParent();
    Map[ToJit] = Args;

    AsCall->eraseFromParent();
  }

  return Map;
}

llvm::SmallVector<llvm::Function *, 8> GetFunctions(FunToInlineMap const &Map) {
  llvm::SmallVector<llvm::Function *, 8> Ret(Map.size());
  size_t idx = 0;
  for (auto const &pair : Map) {
    Ret[idx++] = pair.first;
  }
  return Ret;
}
