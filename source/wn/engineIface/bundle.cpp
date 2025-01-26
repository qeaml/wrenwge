#include "../engineIface.hpp"
#include "../ScriptRuntime.hpp"
#include <nwge/data/bundle.hpp>
#include <nwge/dialog.hpp>
#include <nwge/json/parse.hpp>

using namespace nwge;

static void nqTexture(WrenVM *vm)
{
  auto *bundle = bundleInSlot(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  auto *texture = textureInSlot(vm, 2);
  bundle->nqTexture(name, *texture);
}

static void nqAudio(WrenVM *vm)
{
  auto *bundle = bundleInSlot(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  auto *buffer = audioBufferInSlot(vm, 2);
  bundle->nqBuffer(name, *buffer);
}


static bool loadHandle(data::RW &file, AnyPtr handle)
{
  auto *vm = getScriptRuntime()->vm();
  if(!makeRW(vm, file)) {
    return false;
  }

  wrenEnsureSlots(vm, 2);
  auto *rwHandle = wrenGetSlotHandle(vm, 0);
  wrenSetSlotHandle(vm, 1, rwHandle);
  wrenSetSlotHandle(vm, 0, reinterpret_cast<WrenHandle*>(handle));
  auto *call = wrenMakeCallHandle(vm, "load(_)");
  auto res = wrenCall(vm, call);
  wrenReleaseHandle(vm, call);
  wrenReleaseHandle(vm, rwHandle);
  wrenReleaseHandle(vm, reinterpret_cast<WrenHandle*>(handle));
  return res == WREN_RESULT_SUCCESS;
}

static void nqCustom(WrenVM *vm)
{
  auto *bundle = bundleInSlot(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  auto *handle = wrenGetSlotHandle(vm, 2);
  bundle->nqFunc(name, handle, loadHandle);
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindBundleMethod(bool isStatic, const char *signature)
{
  BIND(false, "nqTexture(_,_)", nqTexture)
  BIND(false, "nqAudio(_,_)", nqAudio)
  BIND(false, "nqCustom(_,_)", nqCustom)
  return nullptr;
}

static void bundleAllocate(WrenVM *vm)
{
  auto *bundle = wrenSetSlotNewForeign(vm, 0, 0, sizeof(data::Bundle));
  const char *name = wrenGetSlotString(vm, 1);
  new(bundle) data::Bundle(name);
}

static void bundleFinalize(void *ptr)
{
  auto *bundle = reinterpret_cast<data::Bundle*>(ptr);
  bundle->~Bundle();
}

WrenForeignClassMethods bindBundleClass()
{
  return {bundleAllocate, bundleFinalize};
}
