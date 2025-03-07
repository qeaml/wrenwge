#pragma once

#include "wren.hpp"
#include <nwge/audio/Source.hpp>
#include <nwge/bind.hpp>
#include <nwge/common/maybe.hpp>
#include <nwge/data/bundle.hpp>
#include <nwge/data/store.hpp>

WrenForeignMethodFn bindForeignMethod(
  WrenVM* vm,
  const char* module,
  const char* className,
  bool isStatic,
  const char* signature);

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature);
WrenForeignMethodFn bindStateMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindAudioSourceMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindSubStateMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindKeyBindMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindConsoleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindBundleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindRwMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindStoreMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindJsonMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature);

WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className);

WrenForeignClassMethods bindEngineClass(const char *className);
WrenForeignClassMethods bindAudioBufferClass();
WrenForeignClassMethods bindAudioSourceClass();
WrenForeignClassMethods bindKeyBindClass();
WrenForeignClassMethods bindBundleClass();
WrenForeignClassMethods bindRwClass();
WrenForeignClassMethods bindStoreClass();
WrenForeignClassMethods bindTextureClass();

inline nwge::audio::Buffer *audioBufferInSlot(WrenVM *vm, int slot)
{
  return reinterpret_cast<nwge::audio::Buffer*>(wrenGetSlotForeign(vm, slot));
}

inline nwge::audio::Source *audioSourceInSlot(WrenVM *vm, int slot)
{
  return reinterpret_cast<nwge::audio::Source*>(wrenGetSlotForeign(vm, slot));
}

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

inline nwge::data::RW *rwInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<nwge::data::RW*>(wrenGetSlotForeign(vm, slot));
}

bool makeRW(WrenVM *vm, nwge::data::RW &file);

inline nwge::data::Store *storeInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<nwge::data::Store*>(wrenGetSlotForeign(vm, slot));
}

inline nwge::render::Texture *textureInSlot(WrenVM *vm, int slot) {
  return reinterpret_cast<nwge::render::Texture*>(wrenGetSlotForeign(vm, slot));
}
