#include "../engineIface.hpp"
#include <nwge/data/store.hpp>

using namespace nwge;

static void nqLoad(WrenVM *vm)
{
  auto *store = storeInSlot(vm, 0);
  const auto *name = wrenGetSlotString(vm, 1);
  auto *handle = wrenGetSlotHandle(vm, 2);
  store->nqLoad(name, [vm, handle](data::RW &file){
    if(!makeRW(vm, file)) {
      return false;
    }

    wrenEnsureSlots(vm, 2);
    auto *rwHandle = wrenGetSlotHandle(vm, 0);
    wrenSetSlotHandle(vm, 1, rwHandle);
    wrenSetSlotHandle(vm, 0, handle);
    auto *call = wrenMakeCallHandle(vm, "load(_)");
    auto res = wrenCall(vm, call);
    wrenReleaseHandle(vm, call);
    wrenReleaseHandle(vm, rwHandle);
    wrenReleaseHandle(vm, handle);
    return res == WREN_RESULT_SUCCESS;
  });
}

static void nqSave(WrenVM *vm)
{
  auto *store = storeInSlot(vm, 0);
  const auto *name = wrenGetSlotString(vm, 1);
  auto *handle = wrenGetSlotHandle(vm, 2);
  store->nqSave(name, [vm, handle](data::RW &file){
    if(!makeRW(vm, file)) {
      return false;
    }

    wrenEnsureSlots(vm, 2);
    auto *rwHandle = wrenGetSlotHandle(vm, 0);
    wrenSetSlotHandle(vm, 1, rwHandle);
    wrenSetSlotHandle(vm, 0, handle);
    auto *call = wrenMakeCallHandle(vm, "save(_)");
    auto res = wrenCall(vm, call);
    wrenReleaseHandle(vm, call);
    wrenReleaseHandle(vm, rwHandle);
    wrenReleaseHandle(vm, handle);
    return res == WREN_RESULT_SUCCESS;
  });
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindStoreMethod(bool isStatic, const char *signature)
{
  BIND(false, "nqLoad(_,_)", nqLoad)
  BIND(false, "nqSave(_,_)", nqSave)
  return nullptr;
}

static void storeAllocate(WrenVM *vm)
{
  auto *store = wrenSetSlotNewForeign(vm, 0, 0, sizeof(data::Store));
  int length;
  ConstCStr ptr = wrenGetSlotBytes(vm, 1, &length);
  if(length == 0) {
    new(store) data::Store();
  } else {
    new(store) data::Store(StringView{ptr, static_cast<usize>(length)});
  }
}

static void storeFinalize(void *data)
{
  auto *store = reinterpret_cast<data::Store*>(data);
  store->~Store();
}

WrenForeignClassMethods bindStoreClass()
{
  return {storeAllocate, storeFinalize};
}
