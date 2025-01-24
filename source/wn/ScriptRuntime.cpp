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
    if(mEventClass != nullptr) {
      wrenReleaseHandle(mVM, mEventClass);
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
  wrenGetVariable(mVM, "engine", "Event", 0);
  mEventClass = wrenGetSlotHandle(mVM, 0);

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

bool ScriptRuntime::on(nwge::Event &evt, WrenHandle *state)
{
  /* used for SubStates, use main state if nullptr */
  if(state == nullptr) {
    state = mCurrStateHandle;
    /* ignore all events if we haven't initialized yed */
    if(mCurrStateHandle == nullptr || mNextStateHandle != nullptr) {
      return true;
    }
  }

  switch(evt.type) {
  case Event::KeyDown:
  case Event::KeyUp:
    return true;

  case Event::MouseMotion:
    return fwdMouseMotion(state, evt.motion);

  case Event::MouseDown:
    return fwdMouseClick(state, true, evt.click);

  case Event::MouseUp:
    return fwdMouseClick(state, false, evt.click);

  case Event::MouseScroll:
    return fwdMouseScroll(state, evt.scroll);

  case Event::PostLoad:
  case Event::Max:
    return true;
  }
}

bool ScriptRuntime::fwdMouseMotion(WrenHandle *state, const MouseMotion &motion)
{
  /*
  0 -> Event class
  1 -> from list
  2 -> to list
  3 -> delta list
  4 -> temporary
  = 5
  */
  wrenEnsureSlots(mVM, 5);
  wrenSetSlotHandle(mVM, 0, mEventClass);

  /* from list */
  wrenSetSlotNewList(mVM, 1);
  wrenSetSlotDouble(mVM, 4, motion.from.x);
  wrenInsertInList(mVM, 1, -1, 4);
  wrenSetSlotDouble(mVM, 4, motion.from.y);
  wrenInsertInList(mVM, 1, -1, 4);

  /* to list */
  wrenSetSlotNewList(mVM, 2);
  wrenSetSlotDouble(mVM, 4, motion.to.x);
  wrenInsertInList(mVM, 2, -1, 4);
  wrenSetSlotDouble(mVM, 4, motion.to.y);
  wrenInsertInList(mVM, 2, -1, 4);

  /* delta list */
  wrenSetSlotNewList(mVM, 3);
  wrenSetSlotDouble(mVM, 4, motion.delta.x);
  wrenInsertInList(mVM, 3, -1, 4);
  wrenSetSlotDouble(mVM, 4, motion.delta.y);
  wrenInsertInList(mVM, 3, -1, 4);

  /* mouseMotion(from,to,delta) constructor */
  auto *call = wrenMakeCallHandle(mVM, "mouseMotion(_,_,_)");
  wrenCall(mVM, call);
  wrenReleaseHandle(mVM, call);

  return fwdEvent(state, wrenGetSlotHandle(mVM, 0));
}

bool ScriptRuntime::fwdMouseClick(WrenHandle *state, bool down, const MouseClick &click)
{
  /*
  0 -> Event class
  1 -> pos list
  2 -> button
  3 -> temporary
  = 4
  */
  wrenEnsureSlots(mVM, 4);
  wrenSetSlotHandle(mVM, 0, mEventClass);

  /* pos list */
  wrenSetSlotNewList(mVM, 1);
  wrenSetSlotDouble(mVM, 3, click.pos.x);
  wrenInsertInList(mVM, 1, -1, 3);
  wrenSetSlotDouble(mVM, 3, click.pos.y);
  wrenInsertInList(mVM, 1, -1, 3);

  /* button */
  wrenSetSlotDouble(mVM, 2, static_cast<f64>(click.button));

  /* mouseDown or mouseUp constructor */
  WrenHandle *call;
  if(down) {
    call = wrenMakeCallHandle(mVM, "mouseDown(_,_)");
  } else {
    call = wrenMakeCallHandle(mVM, "mouseUp(_,_)");
  }
  wrenCall(mVM, call);
  wrenReleaseHandle(mVM, call);

  return fwdEvent(state, wrenGetSlotHandle(mVM, 0));
}

bool ScriptRuntime::fwdMouseScroll(WrenHandle *state, s32 amt)
{
  /*
  0 -> Event class
  1 -> amount
  = 2
  */
  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, mEventClass);
  wrenSetSlotDouble(mVM, 1, amt);
  WrenHandle *call = wrenMakeCallHandle(mVM, "mouseScroll(_)");
  wrenCall(mVM, call);
  wrenReleaseHandle(mVM, call);

  return fwdEvent(state, wrenGetSlotHandle(mVM, 0));
}

bool ScriptRuntime::fwdEvent(WrenHandle *state, WrenHandle *event)
{
  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, state);
  wrenSetSlotHandle(mVM, 1, event);
  auto *call = wrenMakeCallHandle(mVM, "on(_)");
  wrenCall(mVM, call);
  wrenReleaseHandle(mVM, call);
  wrenReleaseHandle(mVM, event);
  if(wrenGetSlotType(mVM, 0) == WREN_TYPE_BOOL) {
    return wrenGetSlotBool(mVM, 0);
  }
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
