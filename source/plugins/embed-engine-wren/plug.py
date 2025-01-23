import bip
import string
from pathlib import Path

g_src = Path("source/wn/engine.wren")
g_dst = Path("source/wn/engine.wren.g.cpp")

def configure(cfg: dict) -> bool:
  return True

def want_run() -> bool:
  if not g_dst.exists():
    return True
  if not g_src.exists():
    return False
  return g_src.stat().st_mtime > g_dst.stat().st_mtime

TEMPLATE = string.Template("""
#include "engine.wren.hpp"
const char *const cEngineWrenSource = R"(
${source}
)";
""".strip())

def run() -> bool:
  with g_src.open("r") as f:
    text = f.read()

  with g_dst.open("w") as f:
    f.write(TEMPLATE.substitute(source=text))

  return True