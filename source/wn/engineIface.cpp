#include "engineIface.hpp"
#include <cstring>

WrenForeignMethodFn bindForeignMethod(
  WrenVM* vm,
  const char* module,
  const char* className,
  bool isStatic,
  const char* signature)
{
  if(strcmp(module, "engine") == 0)
  {
    return bindEngineMethod(className, isStatic, signature);
  }
  return nullptr;
}

#define BIND_CLASS(targetClass) \
  if(strcmp(className, #targetClass) == 0) { \
    return bind##targetClass##Method(isStatic, signature); \
  }

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature)
{
  BIND_CLASS(Console)
  BIND_CLASS(Render)
  return nullptr;
}
