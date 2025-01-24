#include "../engineIface.hpp"
#include "../ScriptRuntime.hpp"

using namespace nwge;

static void swapState(WrenVM *vm)
{
  getScriptRuntime()->swapState(wrenGetSlotHandle(vm, 1));
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindStateMethod(bool isStatic, const char *signature)
{
  BIND(true, "swap(_)", swapState)
  return nullptr;
}
