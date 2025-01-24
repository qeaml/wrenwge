#include "ScriptRuntime.hpp"
#include "engine.wren.hpp"
#include "engineIface.hpp"
#include <cstring>
#include <nwge/console.hpp>
#include <nwge/data/bundle.hpp>
#include <nwge/dialog.hpp>

using namespace nwge;

static ScriptRuntime *gScriptRuntime;

ScriptRuntime *getScriptRuntime()
{
  return gScriptRuntime;
}

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
  auto file = gScriptRuntime->bundle().find(fileName);
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

ScriptRuntime::ScriptRuntime(StringView bundle)
  : mBundleFileName(std::move(bundle))
{
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
    mBundleFileName(std::move(other.mBundleFileName)),
    mVM(other.mVM)
{
  other.mVM = nullptr;
}

ScriptRuntime::~ScriptRuntime()
{
  if(mVM != nullptr) {
    if(mCurrStateHandle != nullptr) {
      wrenReleaseHandle(mVM, mCurrStateHandle);
    }
    if(mNextStateHandle != nullptr) {
      wrenReleaseHandle(mVM, mNextStateHandle);
    }
    wrenFreeVM(mVM);
    mVM = nullptr;
  }
  gScriptRuntime = nullptr;
}

void ScriptRuntime::preload()
{
  mBundle.load({mBundleFileName});
}

bool ScriptRuntime::init(const char *initialState)
{
  gScriptRuntime = this;

  auto res = wrenInterpret(mVM, "preload", "import \"main\"");
  if(res != WREN_RESULT_SUCCESS) {
    dialog::error("Script Error", "A script error has occurred.");
    return false;
  }

  wrenEnsureSlots(mVM, 1);
  wrenGetVariable(mVM, "main", initialState, 0);
  auto *newMethod = wrenMakeCallHandle(mVM, "new()");
  res = wrenCall(mVM, newMethod);
  wrenReleaseHandle(mVM, newMethod);
  if(res != WREN_RESULT_SUCCESS) {
    dialog::error("Script Error", "A script error has occurred.");
    return false;
  }

  mNextStateHandle = wrenGetSlotHandle(mVM, 0);
  return true;
}

bool ScriptRuntime::tick(f32 delta)
{
  if(mNextStateHandle != nullptr) {
    return swapToNextState();
  }

  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, mCurrStateHandle);
  auto *tickMethod = wrenMakeCallHandle(mVM, "tick(_)");
  wrenSetSlotDouble(mVM, 1, delta);
  auto res = wrenCall(mVM, tickMethod);
  wrenReleaseHandle(mVM, tickMethod);
  if(res != WREN_RESULT_SUCCESS) {
    dialog::error("Script Error", "A script error has occurred.");
    return false;
  }

  return true;
}

bool ScriptRuntime::swapToNextState()
{
  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, mNextStateHandle);
  auto *initMethod = wrenMakeCallHandle(mVM, "init()");
  auto res = wrenCall(mVM, initMethod);
  wrenReleaseHandle(mVM, initMethod);
  if(res != WREN_RESULT_SUCCESS) {
    dialog::error("Script Error", "A script error has occurred.");
    return false;
  }

  auto *prevStateHandle = mCurrStateHandle;
  mCurrStateHandle = mNextStateHandle;
  mNextStateHandle = nullptr;
  if(prevStateHandle != nullptr) {
    wrenReleaseHandle(mVM, prevStateHandle);
  }
  wrenCollectGarbage(mVM);
  return true;
}

void ScriptRuntime::render() const
{
  wrenEnsureSlots(mVM, 1);
  wrenSetSlotHandle(mVM, 0, mCurrStateHandle);
  auto *renderMethod = wrenMakeCallHandle(mVM, "render()");
  wrenCall(mVM, renderMethod);
  wrenReleaseHandle(mVM, renderMethod);
}

void ScriptRuntime::swapState(WrenHandle *nextStateHandle)
{
  mNextStateHandle = nextStateHandle;
}
