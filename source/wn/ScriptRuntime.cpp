#include "ScriptRuntime.hpp"
#include "wren.hpp"
#include "engine.wren.hpp"
#include "engineIface.hpp"
#include <cstring>
#include <nwge/console.hpp>
#include <nwge/data/bundle.hpp>

using namespace nwge;

static data::Bundle *gBundlePtr;

static WrenLoadModuleResult loadModule(WrenVM *vm, const char *name)
{
  WrenLoadModuleResult result;
  result.source = nullptr;
  result.onComplete = nullptr;
  result.userData = nullptr;

  if(strcmp(name, "engine") == 0) {
    result.source = cEngineWrenSource;
    return result;
  }

  ScratchString fileName = ScratchString::formatted("{}.wren", name);
  auto file = gBundlePtr->find(fileName);
  if(!file) {
    return result;
  }

  s64 size = file.size();
  if(size < 0) {
    return result;
  }

  auto *source = new char[usize(size)+1];
  if(file.read(source, sizeof(char), usize(size)) != usize(size)) {
    delete[] source;
    return result;
  }

  source[usize(size)] = '\0';
  result.source = source;
  result.onComplete = [](WrenVM*, const char*, WrenLoadModuleResult result) {
    delete[] result.source;
  };
  return result;
}

static WrenForeignClassMethods bindForeignClass(
    WrenVM* vm, const char* module, const char* className)
{
  WrenForeignClassMethods methods;
  methods.allocate = nullptr;
  methods.finalize = nullptr;
  return methods;
}

static void write(WrenVM* vm, const char* text)
{
  console::print(text);
}

static void error(
  WrenVM* vm,
  WrenErrorType type,
  const char* module,
  int line,
  const char* message)
{
  const auto *moduleName = module == nullptr ? "???" : module;
  console::error("{}:{} : {}", moduleName, line, message);
}

static void *reallocate(void *memory, size_t newSize, void *userData)
{
  if(memory == nullptr) {
    return nwgeAlloc(newSize);
  }
  if(newSize == 0) {
    nwgeFree(memory);
    return nullptr;
  }
  return nwgeRealloc(memory, newSize);
}

ScriptRuntime::ScriptRuntime(const StringView &bundle)
{
  mBundle.load({bundle});
  gBundlePtr = &mBundle;

  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.loadModuleFn = &loadModule;
  config.bindForeignMethodFn = &bindForeignMethod;
  config.bindForeignClassFn = &bindForeignClass;
  config.writeFn = &write;
  config.errorFn = &error;
  config.reallocateFn = &reallocate;

  mVM = wrenNewVM(&config);
}

ScriptRuntime::ScriptRuntime(ScriptRuntime &&other) noexcept
  : mBundle(std::move(other.mBundle)),
    mVM(other.mVM)
{
  other.mVM = nullptr;
}

ScriptRuntime::~ScriptRuntime()
{
  if(mVM != nullptr) {
    wrenFreeVM(mVM);
    mVM = nullptr;
  }
}