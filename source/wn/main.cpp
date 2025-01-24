#include <nwge/engine.hpp>
#include <nwge/bndl/reader.h>
#include <nwge/cli/cli.h>
#include <nwge/dialog.hpp>
#include <nwge/json/parse.hpp>
#include <nwge/json/Schema.hpp>
#include "ScriptState.hpp"
#include "config.hpp"

using namespace nwge;

s32 main(s32 argc, CStr *argv)
{
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

  auto config = parseConfig(*res);
  if(!config.present()) {
    return 1;
  }

  initScriptRuntime(fileName);
  atexit(deleteScriptRuntime);
  startPtr(
    createScriptState(config->initialState.data()),
    std::move(config->engineConfig)
  );
  return 0;
}
