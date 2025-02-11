#include "../engineIface.hpp"
#include <nwge/common/slice.hpp>
#include <nwge/data/FileView.hpp>

using namespace nwge;

static void readString(WrenVM *vm)
{
  auto *rw = rwInSlot(vm, 0);
  data::FileView view{*rw};
  if(!view.ok()) {
    wrenSetSlotBytes(vm, 0, "", 0);
    return;
  }
  auto data = view.data();
  wrenSetSlotBytes(vm, 0, data.data(), data.size());
}

static ScratchArray<char> getBytes(WrenVM *vm, int slot)
{
  if(wrenGetSlotType(vm, slot) == WREN_TYPE_STRING) {
    int length;
    ConstCStr ptr = wrenGetSlotBytes(vm, slot, &length);
    return ArrayView<const char>{ptr, static_cast<usize>(length)};
  }
  if(wrenGetSlotType(vm, slot) != WREN_TYPE_LIST) {
    return {};
  }
  wrenEnsureSlots(vm, slot+2);
  int count = wrenGetListCount(vm, slot);
  ScratchSlice<char> bytes{static_cast<usize>(count)};
  for(int i = 0; i < count; ++i) {
    wrenGetListElement(vm, slot, i, slot+1);
    if(wrenGetSlotType(vm, 1) != WREN_TYPE_NUM) {
      return {};
    }
    bytes.push(static_cast<char>(wrenGetSlotDouble(vm, slot+1)));
  }
  return bytes.view();
}

static void write(WrenVM *vm)
{
  auto *rw = rwInSlot(vm, 0);
  auto bytes = getBytes(vm, 1);
  if(bytes.empty()) {
    return;
  }
  rw->write(bytes.view());
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindRwMethod(bool isStatic, const char *signature)
{
  BIND(false, "readString()", readString)
  BIND(false, "write(_)", write)
  return nullptr;
}

static void rwAllocate(WrenVM *vm)
{
  int length;
  ConstCStr ptr = wrenGetSlotBytes(vm, 1, &length);
  NWGE_ASSERT(length == sizeof(SDL_RWops*), "*softly* Don't.");
  SDL_RWops *rwPtr = *reinterpret_cast<SDL_RWops*const*>(ptr);
  auto *rw = wrenSetSlotNewForeign(vm, 0, 0, sizeof(data::RW));
  new(rw) data::RW(rwPtr);
}

static void rwFinalize(void *data)
{
  auto *rw = reinterpret_cast<data::RW*>(data);
  rw->~RW();
}

WrenForeignClassMethods bindRwClass()
{
  return {rwAllocate, rwFinalize};
}
