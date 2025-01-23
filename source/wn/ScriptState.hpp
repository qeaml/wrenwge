#pragma once

#include "ScriptRuntime.hpp"
#include <nwge/state.hpp>

nwge::State *createScriptState(ScriptRuntime &&runtime, const char *initialState);
