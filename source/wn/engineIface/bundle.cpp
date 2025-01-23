#include "../engineIface.hpp"
#include "wren.h"
#include <nwge/data/bundle.hpp>

using namespace nwge;

static void bundleNqTexture(WrenVM *vm)
{
  auto *bundle = (data::Bundle **)wrenGetSlotForeign(vm, 0);
  const char *name = wrenGetSlotString(vm, 1);
  auto *texture = (render::Texture **)wrenGetSlotForeign(vm, 2);
  (*bundle)->nqTexture(name, **texture);
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
  auto *bundle = (data::Bundle **)wrenSetSlotNewForeign(vm, 0, 0, sizeof(data::Bundle *));
  const char *name = wrenGetSlotString(vm, 1);
  *bundle = new data::Bundle(name);
}

static void bundleFinalize(void *data)
{
  auto *bundle = (data::Bundle **)data;
  delete *bundle;
}

WrenForeignClassMethods bindBundleClass()
{
  return {bundleAllocate, bundleFinalize};
}
