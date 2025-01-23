#pragma once

#include "wren.hpp"

WrenForeignMethodFn bindForeignMethod(
  WrenVM* vm,
  const char* module,
  const char* className,
  bool isStatic,
  const char* signature);

WrenForeignMethodFn bindEngineMethod(const char *className, bool isStatic, const char *signature);
WrenForeignMethodFn bindConsoleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindBundleMethod(bool isStatic, const char *signature);
WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature);

WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className);

WrenForeignClassMethods bindEngineClass(const char *className);
WrenForeignClassMethods bindBundleClass();
WrenForeignClassMethods bindTextureClass();
