#pragma once

#include "wren.hpp"
#include <nwge/bind.hpp>
#include <nwge/common/maybe.hpp>
#include <nwge/data/bundle.hpp>

WrenForeignMethodFn bindForeignMethod(
  WrenVM* vm,
  const char* module,
  const char* className,
  bool isStatic,
  const char* signature);

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature);
WrenForeignMethodFn bindStateMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindSubStateMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindKeyBindMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindConsoleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindBundleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature);

WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className);

WrenForeignClassMethods bindEngineClass(const char *className);
WrenForeignClassMethods bindKeyBindClass();
WrenForeignClassMethods bindBundleClass();
WrenForeignClassMethods bindTextureClass();

struct KeyBindWrapper {
  nwge::KeyBind bind;
  WrenVM *vm;
  WrenHandle *pressCb;
  WrenHandle *releaseCb = nullptr;
};

inline KeyBindWrapper *keyBindInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<KeyBindWrapper*>(wrenGetSlotForeign(vm, slot));
}

inline nwge::data::Bundle *bundleInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<nwge::data::Bundle*>(wrenGetSlotForeign(vm, slot));
}

inline nwge::render::Texture *textureInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<nwge::render::Texture*>(wrenGetSlotForeign(vm, slot));
}
