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
  bool on(nwge::Event &evt);
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

  bool fwdMouseMotion(const nwge::MouseMotion &motion);
  bool fwdMouseClick(bool down, const nwge::MouseClick &click);
  bool fwdMouseScroll(s32 amt);
  bool fwdEvent(WrenHandle *event);
};

// gets ScriptRuntime if a valid one exists
ScriptRuntime *getScriptRuntime();
