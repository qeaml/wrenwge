#include "ScriptState.hpp"
#include <nwge/console.hpp>
#include <nwge/dialog.hpp>

using namespace nwge;

class ScriptState: public State {
public:
  ScriptState(ScriptRuntime &&runtime, const char *initialState)
    : State("ScriptState"_sv),
      mRuntime(std::move(runtime)),
      mInitialState(initialState)
  {}

  bool init() override {
    auto res = wrenInterpret(mRuntime.vm(), "preload", "import \"main\"");
    if(res != WREN_RESULT_SUCCESS) {
      dialog::error("Script Error", "A script error has occurred.");
      return false;
    }

    wrenEnsureSlots(mRuntime.vm(), 1);
    wrenGetVariable(mRuntime.vm(), "main", mInitialState, 0);
    auto *newMethod = wrenMakeCallHandle(mRuntime.vm(), "new()");
    res = wrenCall(mRuntime.vm(), newMethod);
    wrenReleaseHandle(mRuntime.vm(), newMethod);
    if(res != WREN_RESULT_SUCCESS) {
      dialog::error("Script Error", "A script error has occurred.");
      return false;
    }

    mStateHandle = wrenGetSlotHandle(mRuntime.vm(), 0);
    wrenEnsureSlots(mRuntime.vm(), 2);
    auto *initMethod = wrenMakeCallHandle(mRuntime.vm(), "init()");
    res = wrenCall(mRuntime.vm(), initMethod);
    wrenReleaseHandle(mRuntime.vm(), initMethod);
    if(res != WREN_RESULT_SUCCESS) {
      dialog::error("Script Error", "A script error has occurred.");
      return false;
    }

    return true;
  }

  bool tick(f32 delta) override {
    wrenEnsureSlots(mRuntime.vm(), 2);
    wrenSetSlotHandle(mRuntime.vm(), 0, mStateHandle);
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
