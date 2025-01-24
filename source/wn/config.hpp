#pragma once

#include <nwge/common/maybe.hpp>
#include <nwge/engineConfig.hpp>
#include <nwge/json/Value.hpp>

struct Config {
  nwge::config::Dev engineConfig;
  nwge::String<> initialState;
};

nwge::Maybe<Config> parseConfig(const nwge::json::Value &value);
