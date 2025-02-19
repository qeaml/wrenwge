#include "ScriptState.hpp"
#include <nwge/console.hpp>
#include <nwge/dialog.hpp>

using namespace nwge;

class ScriptState: public State {
public:
  ScriptState(const char *initialState)
    : State("ScriptState"_sv),
      mRuntime(*getScriptRuntime()),
      mInitialState(initialState)
  {}

  bool preload() override {
    mRuntime.preload();
    return true;
  }

  bool init() override {
    mRuntime.init(mInitialState);
    return true;
  }

  bool on(Event &evt) override {
    return mRuntime.on(evt, nullptr);
  }

  bool tick(f32 delta) override {
    return mRuntime.tick(delta);
  }

  void render() const override {
    mRuntime.render();
  }

private:
  ScriptRuntime &mRuntime;
  const char *mInitialState;
};

State *createScriptState(const char *initialState) {
  return new ScriptState(initialState);
}
