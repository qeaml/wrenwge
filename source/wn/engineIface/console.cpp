#include "../engineIface.hpp"
#include <nwge/console.hpp>

using namespace nwge;

static void clear([[maybe_unused]] WrenVM *vm)
{
  console::clear();
}

static void nextLine([[maybe_unused]] WrenVM *vm)
{
  console::nextLine();
}

static void print([[maybe_unused]] WrenVM *vm)
{
  auto color = static_cast<console::Color>(wrenGetSlotDouble(vm, 1));
  console::print(color, wrenGetSlotString(vm, 2));
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindConsoleMethod(bool isStatic, const char *signature)
{
  BIND(true, "clear()", clear)
  BIND(true, "nextLine()", nextLine)
  BIND(true, "print(_,_)", print)
  return nullptr;
}
