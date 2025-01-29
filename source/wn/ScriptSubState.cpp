#include "ScriptSubState.hpp"
#include "wren.h"
#include <nwge/dialog.hpp>

using namespace nwge;

class ScriptSubState: public nwge::SubState {
public:
  ScriptSubState(WrenHandle *subStateHandle)
    : mRuntime(*getScriptRuntime()),
      mSubStateHandle(subStateHandle),
      mTickMethod(wrenMakeCallHandle(mRuntime.vm(), "tick(_)")),
      mRenderMethod(wrenMakeCallHandle(mRuntime.vm(), "render()"))
  {}

  ~ScriptSubState()
  {
    wrenReleaseHandle(mRuntime.vm(), mTickMethod);
    wrenReleaseHandle(mRuntime.vm(), mRenderMethod);
    wrenReleaseHandle(mRuntime.vm(), mSubStateHandle);
    wrenCollectGarbage(mRuntime.vm());
  }

  bool on(Event &evt) override
  {
    return mRuntime.on(evt, mSubStateHandle);
  }

  bool tick(f32 delta) override
  {
    wrenEnsureSlots(mRuntime.vm(), 2);
    wrenSetSlotHandle(mRuntime.vm(), 0, mSubStateHandle);
    wrenSetSlotDouble(mRuntime.vm(), 1, delta);
    auto res = wrenCall(mRuntime.vm(), mTickMethod);
    if(res != WREN_RESULT_SUCCESS) {
      dialog::error("Script Error", "A script error has occurred.");
      return false;
    }
    return true;
  }

  void render() const override
  {
    wrenEnsureSlots(mRuntime.vm(), 1);
    wrenSetSlotHandle(mRuntime.vm(), 0, mSubStateHandle);
    wrenCall(mRuntime.vm(), mRenderMethod);
  }

private:
  ScriptRuntime &mRuntime;
  WrenHandle *mSubStateHandle;
  WrenHandle *mTickMethod;
  WrenHandle *mRenderMethod;
};

SubState *createScriptSubState(WrenHandle *subStateHandle)
{
  return new ScriptSubState(subStateHandle);
}
