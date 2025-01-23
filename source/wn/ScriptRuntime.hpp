#include "wren.hpp"
#include <nwge/data/bundle.hpp>

class ScriptRuntime {
public:
  ScriptRuntime(const nwge::StringView &bundle);
  ScriptRuntime(ScriptRuntime &&other) noexcept;
  ~ScriptRuntime();

  [[nodiscard]]
  WrenVM *vm() const { return mVM; }

private:
  nwge::data::Bundle mBundle;
  WrenVM *mVM = nullptr;
};
