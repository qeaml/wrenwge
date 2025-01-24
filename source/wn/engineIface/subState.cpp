#include "../engineIface.hpp"
#include "../ScriptSubState.hpp"

using namespace nwge;

void push(WrenVM *vm)
{
  auto *subStateHandle = wrenGetSlotHandle(vm, 1);
  pushSubStatePtr(createScriptSubState(subStateHandle));
}

void swap(WrenVM *vm)
{
  auto *subStateHandle = wrenGetSlotHandle(vm, 1);
  swapSubStatePtr(createScriptSubState(subStateHandle));
}

void pop([[maybe_unused]] WrenVM *vm)
{
  popSubState();
}

void clear([[maybe_unused]] WrenVM *vm)
{
  clearSubStates();
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindSubStateMethod(bool isStatic, const char *signature)
{
  BIND(true, "push(_)", push);
  BIND(true, "swap(_)", swap);
  BIND(true, "pop()", pop);
  BIND(true, "clear()", clear);
  return nullptr;
}
