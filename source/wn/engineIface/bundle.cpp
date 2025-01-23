#include "../engineIface.hpp"
#include <nwge/data/bundle.hpp>

using namespace nwge;

static void bundleNqTexture(WrenVM *vm)
{
  auto *bundle = bundleInSlot(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  auto *texture = textureInSlot(vm, 2);
  bundle->nqTexture(name, *texture);
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindBundleMethod(bool isStatic, const char *signature)
{
  BIND(false, "nqTexture(_,_)", bundleNqTexture)
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
