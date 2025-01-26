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

#define BIND_METHOD_X(scriptClass, nativeClass) \
  if(strcmp(className, #scriptClass) == 0) { \
    return bind##nativeClass##Method(isStatic, signature); \
  }

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature)
{
  BIND_METHOD(State)
  BIND_METHOD(SubState)
  BIND_METHOD(AudioSource)
  BIND_METHOD(KeyBind)
  BIND_METHOD(Console)
  BIND_METHOD(Bundle)
  BIND_METHOD_X(RW, Rw)
  BIND_METHOD(Store)
  BIND_METHOD_X(JSON, Json)
  BIND_METHOD(Render)
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

#define BIND_CLASS_X(scriptClass, nativeClass) \
  if(strcmp(className, #scriptClass) == 0) { \
    return bind##nativeClass##Class(); \
  }

WrenForeignClassMethods bindEngineClass(const char *className)
{
  BIND_CLASS(AudioBuffer)
  BIND_CLASS(AudioSource)
  BIND_CLASS(KeyBind)
  BIND_CLASS(Bundle)
  BIND_CLASS_X(RW, Rw)
  BIND_CLASS(Store)
  BIND_CLASS(Texture)
  return {nullptr, nullptr};
}

/*
Trigger Warning: Pointer stored as string, blind memset
*/
bool makeRW(WrenVM *vm, nwge::data::RW &file)
{
  wrenEnsureSlots(vm, 2);
  wrenGetVariable(vm, "engine", "RW", 0);
  /* ew */
  SDL_RWops *rwPtr = &*file;
  wrenSetSlotBytes(vm, 1, reinterpret_cast<ConstCStr>(&rwPtr), sizeof(SDL_RWops*));
  auto *call = wrenMakeCallHandle(vm, "internalConstructorDoNotUse(_)");
  auto res = wrenCall(vm, call);
  wrenReleaseHandle(vm, call);
  if(res != WREN_RESULT_SUCCESS) {
    return false;
  }
  /* EW */
  SDL_memset(&file, 0, sizeof(nwge::data::RW));
  return true;
}
