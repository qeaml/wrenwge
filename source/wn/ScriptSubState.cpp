#include "ScriptSubState.hpp"
#include <nwge/dialog.hpp>

using namespace nwge;

class ScriptSubState: public nwge::SubState {
public:
  ScriptSubState(WrenHandle *subStateHandle)
    : mRuntime(*getScriptRuntime()),
      mSubStateHandle(subStateHandle)
  {}

  ~ScriptSubState()
  {
    wrenReleaseHandle(mRuntime.vm(), mSubStateHandle);
    wrenCollectGarbage(mRuntime.vm());
  }

  bool tick(f32 delta) override
  {
    wrenEnsureSlots(mRuntime.vm(), 2);
    wrenSetSlotHandle(mRuntime.vm(), 0, mSubStateHandle);
    auto *tickMethod = wrenMakeCallHandle(mRuntime.vm(), "tick(_)");
    wrenSetSlotDouble(mRuntime.vm(), 1, delta);
    auto res = wrenCall(mRuntime.vm(), tickMethod);
    wrenReleaseHandle(mRuntime.vm(), tickMethod);
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
    auto *renderMethod = wrenMakeCallHandle(mRuntime.vm(), "render()");
    wrenCall(mRuntime.vm(), renderMethod);
    wrenReleaseHandle(mRuntime.vm(), renderMethod);
  }

private:
  ScriptRuntime &mRuntime;
  WrenHandle *mSubStateHandle;
};

SubState *createScriptSubState(WrenHandle *subStateHandle)
{
  return new ScriptSubState(subStateHandle);
}
