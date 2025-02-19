#include "ScriptRuntime.hpp"
#include "engine.wren.hpp"
#include "engineIface.hpp"
#include "version.h"
#include <cmath>
#include <cstring>
#include <nwge/console.hpp>
#include <nwge/common/version.h>
#include <nwge/data/bundle.hpp>
#include <nwge/dialog.hpp>
#include <nwge/render/draw.hpp>
#include <nwge/state.hpp>

using namespace nwge;

static ScriptRuntime *gScriptRuntime;

void initScriptRuntime(StringView bundle)
{
  gScriptRuntime = new ScriptRuntime(std::move(bundle));
}

ScriptRuntime *getScriptRuntime()
{
  return gScriptRuntime;
}

void deleteScriptRuntime()
{
  delete gScriptRuntime;
  gScriptRuntime = nullptr;
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

void ScriptRuntime::error(WrenErrorType type, const char *module, int line, const char *message)
{
  switch(type) {
  case WREN_ERROR_COMPILE:
    mCompileError = true;
    mErrorMessage.clear();
    mErrorMessage.append(ScratchString::formatted(
      "{}:{}: {}",
      module, line, message));
    break;

  case WREN_ERROR_RUNTIME:
    mRuntimeError = true;
    mErrorMessage.clear();
    mErrorMessage.append(ScratchString::formatted(
      "{}\n"
      "Stack trace:\n",
      message));
    break;

  case WREN_ERROR_STACK_TRACE:
    mErrorMessage.append(ScratchString::formatted(
      "  {}:{} in {}\n",
      module, line, message));
    break;
  }
}

static void error(
  [[maybe_unused]] WrenVM* vm,
  WrenErrorType type,
  const char* module,
  int line,
  const char* message)
{
  gScriptRuntime->error(type, module, line, message);
}

static void *reallocate(void *memory, size_t newSize, void *userData)
{
  if(memory == nullptr) {
    if(newSize == 0) {
      return nullptr;
    }
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
  config.loadModuleFn = &::loadModule;
  config.bindForeignMethodFn = &::bindForeignMethod;
  config.bindForeignClassFn = &::bindForeignClass;
  config.writeFn = &::write;
  config.errorFn = &::error;
  config.reallocateFn = &::reallocate;

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
    if(mCurrStateHandle != nullptr) { wrenReleaseHandle(mVM, mCurrStateHandle); }
    if(mNextStateHandle != nullptr) { wrenReleaseHandle(mVM, mNextStateHandle); }
    if(mEventClass != nullptr) { wrenReleaseHandle(mVM, mEventClass); }
    if(mInitMethod != nullptr) { wrenReleaseHandle(mVM, mInitMethod); }
    if(mMouseMotionMethod != nullptr) { wrenReleaseHandle(mVM, mMouseMotionMethod); }
    if(mMouseDownMethod != nullptr) { wrenReleaseHandle(mVM, mMouseDownMethod); }
    if(mMouseUpMethod != nullptr) { wrenReleaseHandle(mVM, mMouseUpMethod); }
    if(mMouseScrollMethod != nullptr) { wrenReleaseHandle(mVM, mMouseScrollMethod); }
    if(mOnMethod != nullptr) { wrenReleaseHandle(mVM, mOnMethod); }
    if(mTickMethod != nullptr) { wrenReleaseHandle(mVM, mTickMethod); }
    if(mRenderMethod != nullptr) { wrenReleaseHandle(mVM, mRenderMethod); }
    wrenFreeVM(mVM);
    mVM = nullptr;
  }
  gScriptRuntime = nullptr;
}

void ScriptRuntime::preload()
{
  mBundle.load({mBundleFileName});
}

void ScriptRuntime::init(const char *initialState)
{
  gScriptRuntime = this;

  auto nwgeVer = nwgeVersion();
  console::print("WreNwge v" WN_VER_STR ": Wren v" WREN_VERSION_STRING " + Nwge v{}.{}",
    nwgeVer.major, nwgeVer.minor);
  console::print("Note: WreNwge uses a slightly modified version of Wren."_sv);

  auto res = wrenInterpret(mVM, "preload", "import \"main\"");
  if(res != WREN_RESULT_SUCCESS) {
    return;
  }

  wrenEnsureSlots(mVM, 1);
  wrenGetVariable(mVM, "engine", "Event", 0);
  mEventClass = wrenGetSlotHandle(mVM, 0);

  wrenGetVariable(mVM, "main", initialState, 0);
  auto *newMethod = wrenMakeCallHandle(mVM, "new()");
  res = wrenCall(mVM, newMethod);
  wrenReleaseHandle(mVM, newMethod);
  if(res != WREN_RESULT_SUCCESS) {
    return;
  }

  mInitMethod = wrenMakeCallHandle(mVM, "init()");
  mMouseMotionMethod = wrenMakeCallHandle(mVM, "mouseMotion(_,_,_)");
  mMouseDownMethod = wrenMakeCallHandle(mVM, "mouseDown(_,_)");
  mMouseUpMethod = wrenMakeCallHandle(mVM, "mouseUp(_,_)");
  mMouseScrollMethod = wrenMakeCallHandle(mVM, "mouseScroll(_)");
  mOnMethod = wrenMakeCallHandle(mVM, "on(_)");
  mTickMethod = wrenMakeCallHandle(mVM, "tick(_)");
  mRenderMethod = wrenMakeCallHandle(mVM, "render()");

  mNextStateHandle = wrenGetSlotHandle(mVM, 0);
}

bool ScriptRuntime::on(nwge::Event &evt, WrenHandle *state)
{
  if(mCompileError || mRuntimeError) {
    return true;
  }

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
  wrenCall(mVM, mMouseMotionMethod);

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
    call = mMouseDownMethod;
  } else {
    call = mMouseUpMethod;
  }
  wrenCall(mVM, call);

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
  wrenCall(mVM, mMouseScrollMethod);

  return fwdEvent(state, wrenGetSlotHandle(mVM, 0));
}

bool ScriptRuntime::fwdEvent(WrenHandle *state, WrenHandle *event)
{
  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, state);
  wrenSetSlotHandle(mVM, 1, event);
  wrenCall(mVM, mOnMethod);
  wrenReleaseHandle(mVM, event);
  if(wrenGetSlotCount(mVM) > 0) {
    if(wrenGetSlotType(mVM, 0) == WREN_TYPE_BOOL) {
      return wrenGetSlotBool(mVM, 0);
    }
  }
  return true;
}

bool ScriptRuntime::tick(f32 delta)
{
  if(mCompileError || mRuntimeError) {
    mErrorTimer += delta;
    return true;
  }

  if(mNextStateHandle != nullptr) {
    return swapToNextState();
  }

  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, mCurrStateHandle);
  wrenSetSlotDouble(mVM, 1, delta);
  auto res = wrenCall(mVM, mTickMethod);
  if(res != WREN_RESULT_SUCCESS) {
    return true;
  }
  if(wrenGetSlotCount(mVM) > 0) {
    if(wrenGetSlotType(mVM, 0) == WREN_TYPE_BOOL) {
      return wrenGetSlotBool(mVM, 0);
    }
  }

  return true;
}

bool ScriptRuntime::swapToNextState()
{
  wrenEnsureSlots(mVM, 2);
  wrenSetSlotHandle(mVM, 0, mNextStateHandle);
  auto res = wrenCall(mVM, mInitMethod);
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
  if(mCompileError) {
    renderCompileError();
    return;
  }
  if(mRuntimeError) {
    renderRuntimeError();
    return;
  }

  wrenEnsureSlots(mVM, 1);
  wrenSetSlotHandle(mVM, 0, mCurrStateHandle);
  wrenCall(mVM, mRenderMethod);
}

static constexpr f32
  cErrorBannerX = 0.0f,
  cErrorBannerY = 0.0f,
  cErrorBannerZ = 0.15f,
  cErrorBannerW = 1.0f,
  cErrorBannerH = 0.4f,
  cErrorBannerPadding = 0.03f,
  cErrorBannerStartShade = 1.0f,
  cErrorBannerEndShade = 0.1f,
  cErrorBannerShadeTime = 2.0f,
  cErrorTitleX = cErrorBannerX+cErrorBannerPadding,
  cErrorTitleY = cErrorBannerY+cErrorBannerPadding,
  cErrorTitleZ = 0.1f,
  cErrorTitleHeight = 0.075f,
  cErrorMsgX = cErrorTitleX,
  cErrorMsgY = cErrorTitleY+cErrorTitleHeight,
  cErrorMsgZ = 0.1f,
  cErrorMsgHeight = 0.04f;

static f32 quadraticInterpolate(f32 from, f32 to, f32 t)
{
  return std::lerp(from, to, t*t);
}

static void renderErrorBanner(f32 timer)
{
  f32 shade = cErrorBannerEndShade;
  if(timer < cErrorBannerShadeTime) {
    shade = quadraticInterpolate(
      cErrorBannerStartShade, cErrorBannerEndShade,
      timer/cErrorBannerShadeTime);
  }
  render::color({shade, 0, 0});
  render::rect(
    {cErrorBannerX, cErrorBannerY, cErrorBannerZ},
    {cErrorBannerW, cErrorBannerH});
}

void ScriptRuntime::renderCompileError() const
{
  // render::clear({0, 0, 0});
  renderErrorBanner(mErrorTimer);
  render::color();
  render::text(
    {cErrorTitleX, cErrorTitleY, cErrorTitleZ},
    "Compile Error"_sv, cErrorTitleHeight);
  render::text(
    {cErrorMsgX, cErrorMsgY, cErrorMsgZ},
    mErrorMessage.view(), cErrorMsgHeight);
}

void ScriptRuntime::renderRuntimeError() const
{
  // render::clear({0, 0, 0});
  renderErrorBanner(mErrorTimer);
  render::color();
  render::text(
    {cErrorTitleX, cErrorTitleY, cErrorTitleZ},
    "Runtime Error"_sv, cErrorTitleHeight);
  render::text(
    {cErrorMsgX, cErrorMsgY, cErrorMsgZ},
    mErrorMessage.view(), cErrorMsgHeight);
}

void ScriptRuntime::swapState(WrenHandle *nextStateHandle)
{
  mNextStateHandle = nextStateHandle;
}
