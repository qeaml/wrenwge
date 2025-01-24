#include "../engineIface.hpp"

using namespace nwge;

static void keyName(WrenVM *vm)
{
  auto *bind = keyBindInSlot(vm, 0);
  wrenSetSlotString(vm, 0, bind->bind.keyName().data());
}

static void onRelease(WrenVM *vm)
{
  auto *wrapper = keyBindInSlot(vm, 0);
  auto *callback = wrenGetSlotHandle(vm, 1);
  wrapper->bind.onRelease(KeyBind::Callback([callback, vm]{
    wrenEnsureSlots(vm, 1);
    wrenSetSlotHandle(vm, 0, callback);
    auto *handle = wrenMakeCallHandle(vm, "call()");
    wrenCall(vm, handle);
    wrenReleaseHandle(vm, handle);
    if(wrenGetSlotType(vm, 0) != WREN_TYPE_BOOL) {
      return true;
    }
    return wrenGetSlotBool(vm, 0);
  }));
  wrapper->releaseCb = callback;
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindKeyBindMethod(bool isStatic, const char *signature)
{
  BIND(false, "key()", keyName);
  BIND(false, "onRelease(_)", onRelease);
  return nullptr;
}

static void keyBindAllocate(WrenVM *vm)
{
  auto *wrapper = wrenSetSlotNewForeign(vm, 0, 0, sizeof(KeyBindWrapper));
  const auto *name = wrenGetSlotString(vm, 1);
  const auto *keyName = wrenGetSlotString(vm, 2);
  auto key = keyByName(keyName);
  auto *callback = wrenGetSlotHandle(vm, 3);
  new(wrapper) KeyBindWrapper{
    .bind={
      name, key,
      KeyBind::Callback([callback, vm]{
        wrenEnsureSlots(vm, 1);
        wrenSetSlotHandle(vm, 0, callback);
        auto *handle = wrenMakeCallHandle(vm, "call()");
        wrenCall(vm, handle);
        wrenReleaseHandle(vm, handle);
        if(wrenGetSlotType(vm, 0) != WREN_TYPE_BOOL) {
          return true;
        }
        return wrenGetSlotBool(vm, 0);
      })
    },
    .vm=vm,
    .pressCb=callback
  };
}

static void keyBindFinalize(void *data)
{
  auto *bind = reinterpret_cast<KeyBindWrapper*>(data);
  wrenReleaseHandle(bind->vm, bind->pressCb);
  if(bind->releaseCb != nullptr) {
    wrenReleaseHandle(bind->vm, bind->releaseCb);
  }
}

WrenForeignClassMethods bindKeyBindClass()
{
  return {keyBindAllocate, keyBindFinalize};
}
