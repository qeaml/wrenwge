#include "../engineIface.hpp"
#include "../ScriptSubState.hpp"

using namespace nwge;

SubState::Options mapToOptions(WrenVM *vm, s32 mapSlot, s32 tempSlot)
{
  SubState::Options options;

  wrenSetSlotString(vm, tempSlot, "tickParent");
  if(wrenGetMapContainsKey(vm, mapSlot, tempSlot)) {
    wrenGetMapValue(vm, mapSlot, tempSlot, tempSlot);
    if(wrenGetSlotType(vm, tempSlot) == WREN_TYPE_BOOL) {
      options.tickParent = wrenGetSlotBool(vm, tempSlot);
    }
  }

  wrenSetSlotString(vm, tempSlot, "renderParent");
  if(wrenGetMapContainsKey(vm, mapSlot, tempSlot)) {
    wrenGetMapValue(vm, mapSlot, tempSlot, tempSlot);
    if(wrenGetSlotType(vm, tempSlot) == WREN_TYPE_BOOL) {
      options.renderParent = wrenGetSlotBool(vm, tempSlot);
    }
  }

  wrenSetSlotString(vm, tempSlot, "receiveEvents");
  if(wrenGetMapContainsKey(vm, mapSlot, tempSlot)) {
    wrenGetMapValue(vm, mapSlot, tempSlot, tempSlot);
    if(wrenGetSlotType(vm, tempSlot) == WREN_TYPE_BOOL) {
      options.receiveEvents = wrenGetSlotBool(vm, tempSlot);
    }
  }

  wrenSetSlotString(vm, tempSlot, "propagateEvents");
  if(wrenGetMapContainsKey(vm, mapSlot, tempSlot)) {
    wrenGetMapValue(vm, mapSlot, tempSlot, tempSlot);
    if(wrenGetSlotType(vm, tempSlot) == WREN_TYPE_BOOL) {
      options.propagateEvents = wrenGetSlotBool(vm, tempSlot);
    }
  }

  wrenSetSlotString(vm, tempSlot, "subTimeScale");
  if(wrenGetMapContainsKey(vm, mapSlot, tempSlot)) {
    wrenGetMapValue(vm, mapSlot, tempSlot, tempSlot);
    if(wrenGetSlotType(vm, tempSlot) == WREN_TYPE_NUM) {
      options.subTimeScale = f32(wrenGetSlotDouble(vm, tempSlot));
    }
  }

  return options;
}

void push(WrenVM *vm)
{
  /*
  0 -> SubState class
  1 -> sub-state to push
  2 -> options map
  3 -> temporary
  = 4
  */
  wrenEnsureSlots(vm, 4);
  auto options = mapToOptions(vm, 2, 3);
  auto *subStateHandle = wrenGetSlotHandle(vm, 1);
  pushSubStatePtr(createScriptSubState(subStateHandle), options);
}

void swap(WrenVM *vm)
{
  /*
  0 -> SubState class
  1 -> sub-state to swap to
  2 -> options map
  3 -> temporary
  = 4
  */
  wrenEnsureSlots(vm, 4);
  auto options = mapToOptions(vm, 2, 3);
  auto *subStateHandle = wrenGetSlotHandle(vm, 1);
  swapSubStatePtr(createScriptSubState(subStateHandle), options);
}

void pop([[maybe_unused]] WrenVM *vm)
{
  popSubState();
}

void clear([[maybe_unused]] WrenVM *vm)
{
  clearSubStates();
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindSubStateMethod(bool isStatic, const char *signature)
{
  BIND(true, "push(_,_)", push);
  BIND(true, "swap(_,_)", swap);
  BIND(true, "pop()", pop);
  BIND(true, "clear()", clear);
  return nullptr;
}
