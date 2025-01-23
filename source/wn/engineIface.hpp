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
WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature);
