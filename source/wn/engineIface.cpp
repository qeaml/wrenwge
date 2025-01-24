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

#define BIND_METHOD(targetClass) \
  if(strcmp(className, #targetClass) == 0) { \
    return bind##targetClass##Method(isStatic, signature); \
  }

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature)
{
  BIND_METHOD(KeyBind)
  BIND_METHOD(Console)
  BIND_METHOD(Render)
  BIND_METHOD(Bundle)
  return nullptr;
}

WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className)
{
  if(strcmp(module, "engine") == 0)
  {
    return bindEngineClass(className);
  }
  return {nullptr, nullptr};
}

#define BIND_CLASS(targetClass) \
  if(strcmp(className, #targetClass) == 0) { \
    return bind##targetClass##Class(); \
  }

WrenForeignClassMethods bindEngineClass(const char *className)
{
  BIND_CLASS(KeyBind)
  BIND_CLASS(Bundle)
  BIND_CLASS(Texture)
  return {nullptr, nullptr};
}
