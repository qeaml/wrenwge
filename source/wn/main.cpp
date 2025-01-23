#include <nwge/engine.hpp>
#include <nwge/bndl/reader.h>
#include <nwge/cli/cli.h>
#include <nwge/dialog.hpp>
#include <nwge/json/parse.hpp>
#include <nwge/json/Schema.hpp>
#include "ScriptState.hpp"

using namespace nwge;

s32 main(s32 argc, CStr *argv) {
  cli::parse(argc, argv);

  if(cli::posC() < 1) {
    dialog::error("Engine Error"_sv, "No game specified"_sv);
    return 1;
  }

  ScratchString fileName = ScratchString::formatted("{}.bndl", cli::pos(0));
  // bndlFreeReader() closes this for us
  SDL_RWops *bndlRW = SDL_RWFromFile(fileName.data(), "rb");
  if(bndlRW == nullptr) {
    dialog::error("Engine Error"_sv, "Failed to open game bundle"_sv);
    return 1;
  }

  BndlReader reader;
  BndlErr err = bndlInitReader(&reader, bndlRW);
  if(err != BndlErrOK) {
    dialog::error("Engine Error"_sv,
      "Failed to read game bundle:\n"
      "{}",
      bndlErrorMsg(err));
    bndlFreeReader(&reader);
    return 1;
  }

  BndlFile *gameInfoFile = bndlReaderFind(&reader, "game.json");
  if(gameInfoFile == nullptr) {
    dialog::error("Engine Error"_sv, "Failed to find game.json in bundle"_sv);
    bndlFreeReader(&reader);
    return 1;
  }

  data::RW gameInfoRW = bndlReaderOpen(&reader, gameInfoFile);
  auto res = json::parse(gameInfoRW);
  bndlFreeReader(&reader);
  if(!res) {
    dialog::error("Engine Error"_sv,
      "Failed to parse game.json:\n"
      "{}",
      res.error.message());
    return 1;
  }

  auto root = json::Schema::object(*res.value);
  if(!root.present()) {
    dialog::error("Engine Error"_sv, "game.json is not an object"_sv);
    return 1;
  }

  auto name = root->expectStringField("name"_sv);
  if(!name.present()) {
    dialog::error("Engine Error"_sv, "game.json is missing name field"_sv);
    return 1;
  }

  auto initialStateName = root->expectStringField("initialState"_sv);
  if(!initialStateName.present()) {
    dialog::error("Engine Error"_sv, "game.json is missing initialState field"_sv);
    return 1;
  }

  ScriptRuntime scriptRuntime{fileName};

  startPtr(createScriptState(std::move(scriptRuntime), initialStateName->data()), {
    .loadBlocking = config::LoadBlocking::Always,
    .appName = *name,
  });
  return 0;
}
