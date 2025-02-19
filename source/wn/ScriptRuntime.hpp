#include "wren.hpp"
#include <nwge/common/slice.hpp>
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
  void init(const char *initialState);
  bool on(nwge::Event &evt, WrenHandle *state);
  bool tick(f32 delta);
  void render() const;

  void swapState(WrenHandle *nextStateHandle);

  void error(WrenErrorType type, const char *module, int line, const char *message);

private:
  nwge::data::Bundle mBundle;
  nwge::StringView mBundleFileName;
  WrenVM *mVM = nullptr;

  WrenHandle *mCurrStateHandle = nullptr;
  WrenHandle *mNextStateHandle = nullptr;

  WrenHandle *mInitMethod = nullptr;
  WrenHandle *mMouseMotionMethod = nullptr;
  WrenHandle *mMouseDownMethod = nullptr;
  WrenHandle *mMouseUpMethod = nullptr;
  WrenHandle *mMouseScrollMethod = nullptr;
  WrenHandle *mOnMethod = nullptr;
  WrenHandle *mTickMethod = nullptr;
  WrenHandle *mRenderMethod = nullptr;

  bool swapToNextState();

  WrenHandle *mEventClass = nullptr;

  bool fwdMouseMotion(WrenHandle *state, const nwge::MouseMotion &motion);
  bool fwdMouseClick(WrenHandle *state, bool down, const nwge::MouseClick &click);
  bool fwdMouseScroll(WrenHandle *state, s32 amt);
  bool fwdEvent(WrenHandle *state, WrenHandle *event);

  f32 mErrorTimer = 0.0f;
  bool mCompileError = false;
  bool mRuntimeError = false;
  nwge::Slice<char> mErrorMessage{4};

  void renderCompileError() const;
  void renderRuntimeError() const;
};

void initScriptRuntime(nwge::StringView bundle);
// gets ScriptRuntime if a valid one exists
ScriptRuntime *getScriptRuntime();
void deleteScriptRuntime();
