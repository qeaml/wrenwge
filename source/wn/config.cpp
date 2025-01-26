#include "config.hpp"
#include <nwge/dialog.hpp>
#include <nwge/json/Schema.hpp>

using namespace nwge;

#define FAIL_WITH(...) \
  dialog::error("Engine Error", "Could not parse game config:\n" __VA_ARGS__); \
  return {}

Maybe<Config> parseConfig(const nwge::json::Value &value)
{
  auto root = json::Schema::object(value);
  if(!root.present()) {
    FAIL_WITH("Config is not an object");
  }

  auto initialState = root->expectStringField("initialState"_sv);
  if(!initialState.present()) {
    FAIL_WITH("Config does not define `initialState`");
  }

  auto name = root->expectStringField("name"_sv);
  if(!name.present()) {
    FAIL_WITH("Config does not define `name`");
  }

  Config config{
    .engineConfig={
      .loadBlocking=config::LoadBlocking::Always,
      .appName=*name,
    },
    .initialState=*initialState
  };

  auto windowResizable = root->expectBooleanField("windowResizable"_sv);
  if(windowResizable.present()) {
    config.engineConfig.windowResizable = *windowResizable;
  }
  auto windowBorderless = root->expectBooleanField("windowBorderless"_sv);
  if(windowBorderless.present()) {
    config.engineConfig.windowBorderless = *windowBorderless;
  }
  auto windowAspectRatio = root->expectArrayField("windowAspectRatio"_sv);
  if(windowAspectRatio.present()) {
    auto w = windowAspectRatio->expectNumberElement();
    if(!w.present()) {
      FAIL_WITH("`windowAspectRatio` must contain at least 2 numbers");
    }
    auto h = windowAspectRatio->expectNumberElement();
    if(!h.present()) {
      FAIL_WITH("`windowAspectRatio` must contain at least 2 numbers");
    }
    config.engineConfig.windowAspectRatio = {
      static_cast<s32>(*w),
      static_cast<s32>(*h)
    };
  }

  auto renderResolution = root->expectArrayField("renderResolution"_sv);
  if(renderResolution.present()) {
    auto w = renderResolution->expectNumberElement();
    if(!w.present()) {
      FAIL_WITH("`renderResolution` must contain at least 2 numbers");
    }
    auto h = renderResolution->expectNumberElement();
    if(!h.present()) {
      FAIL_WITH("`renderResolution` must contain at least 2 numbers");
    }
    config.engineConfig.renderResolution = {
      static_cast<s32>(*w),
      static_cast<s32>(*h)
    };
  }
  auto filterTextures = root->expectBooleanField("filterTextures"_sv);
  if(filterTextures.present()) {
    config.engineConfig.filterTextures = *filterTextures;
  }
  auto filterFonts = root->expectBooleanField("filterFonts"_sv);
  if(filterFonts.present()) {
    config.engineConfig.filterFonts = *filterFonts;
  }

  return config;
}