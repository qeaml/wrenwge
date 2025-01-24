#include "wren.hpp"
#include <nwge/data/bundle.hpp>
#include <nwge/event.hpp>

class ScriptRuntime {
public:
  ScriptRuntime(nwge::StringView bundle);
  ScriptRuntime(ScriptRuntime &&other) noexcept;
  ~ScriptRuntime();

  [[nodiscard]]
  WrenVM *vm() const { return mVM; }

  [[nodiscard]]
  const nwge::data::Bundle &bundle() const { return mBundle; }

  void preload();
  bool init(const char *initialState);
  bool on(nwge::Event &evt, WrenHandle *state);
  bool tick(f32 delta);
  void render() const;

  void swapState(WrenHandle *nextStateHandle);

private:
  nwge::data::Bundle mBundle;
  nwge::StringView mBundleFileName;
  WrenVM *mVM = nullptr;

  WrenHandle *mCurrStateHandle = nullptr;
  WrenHandle *mNextStateHandle = nullptr;

  bool swapToNextState();

  WrenHandle *mEventClass = nullptr;

  bool fwdMouseMotion(WrenHandle *state, const nwge::MouseMotion &motion);
  bool fwdMouseClick(WrenHandle *state, bool down, const nwge::MouseClick &click);
  bool fwdMouseScroll(WrenHandle *state, s32 amt);
  bool fwdEvent(WrenHandle *state, WrenHandle *event);
};

void initScriptRuntime(nwge::StringView bundle);
// gets ScriptRuntime if a valid one exists
ScriptRuntime *getScriptRuntime();
void deleteScriptRuntime();
