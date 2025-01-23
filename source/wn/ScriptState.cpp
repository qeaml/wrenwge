#include "ScriptState.hpp"
#include "wren.h"
#include <nwge/console.hpp>

using namespace nwge;

class ScriptState: public State {
public:
  ScriptState(ScriptRuntime &&runtime, const char *initialState)
    : State("ScriptState"_sv),
      mRuntime(std::move(runtime)),
      mInitialState(initialState)
  {}

  bool init() override {
    wrenInterpret(mRuntime.vm(), "preload", "import \"main\"");

    wrenEnsureSlots(mRuntime.vm(), 1);
    wrenGetVariable(mRuntime.vm(), "main", mInitialState, 0);
    auto *newMethod = wrenMakeCallHandle(mRuntime.vm(), "new()");
    wrenCall(mRuntime.vm(), newMethod);
    wrenReleaseHandle(mRuntime.vm(), newMethod);

    mStateHandle = wrenGetSlotHandle(mRuntime.vm(), 0);
    wrenEnsureSlots(mRuntime.vm(), 2);
    auto *initMethod = wrenMakeCallHandle(mRuntime.vm(), "init()");
    wrenCall(mRuntime.vm(), initMethod);
    wrenReleaseHandle(mRuntime.vm(), initMethod);

    return true;
  }

  bool tick(f32 delta) override {
    wrenEnsureSlots(mRuntime.vm(), 2);
    wrenSetSlotHandle(mRuntime.vm(), 0, mStateHandle);
    auto *tickMethod = wrenMakeCallHandle(mRuntime.vm(), "tick(_)");
    wrenSetSlotDouble(mRuntime.vm(), 1, delta);
    wrenCall(mRuntime.vm(), tickMethod);
    wrenReleaseHandle(mRuntime.vm(), tickMethod);

    return true;
  }

  void render() const override {
    wrenEnsureSlots(mRuntime.vm(), 1);
    wrenSetSlotHandle(mRuntime.vm(), 0, mStateHandle);
    auto *renderMethod = wrenMakeCallHandle(mRuntime.vm(), "render()");
    wrenCall(mRuntime.vm(), renderMethod);
    wrenReleaseHandle(mRuntime.vm(), renderMethod);
  }

  ~ScriptState() {
    if(mStateHandle != nullptr) {
      wrenReleaseHandle(mRuntime.vm(), mStateHandle);
    }
  }

private:
  ScriptRuntime mRuntime;
  const char *mInitialState;

  WrenHandle *mStateHandle = nullptr;
};

State *createScriptState(ScriptRuntime &&runtime, const char *initialState) {
  return new ScriptState(std::move(runtime), initialState);
}
