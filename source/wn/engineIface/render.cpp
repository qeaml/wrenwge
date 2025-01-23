#include "../engineIface.hpp"
#include <nwge/render/draw.hpp>

using namespace nwge;

static void clear(WrenVM *vm)
{
  auto red = wrenGetSlotDouble(vm, 1);
  auto green = wrenGetSlotDouble(vm, 2);
  auto blue = wrenGetSlotDouble(vm, 3);
  render::clear({red, green, blue});
}

static void color(WrenVM *vm)
{
  auto red = wrenGetSlotDouble(vm, 1);
  auto green = wrenGetSlotDouble(vm, 2);
  auto blue = wrenGetSlotDouble(vm, 3);
  auto alpha = wrenGetSlotDouble(vm, 4);
  render::color({red, green, blue, alpha});
}

static void rectNoTexture(WrenVM *vm)
{
  auto x = wrenGetSlotDouble(vm, 1);
  auto y = wrenGetSlotDouble(vm, 2);
  auto z = wrenGetSlotDouble(vm, 3);
  auto width = wrenGetSlotDouble(vm, 4);
  auto height = wrenGetSlotDouble(vm, 5);
  render::rect({x, y, z}, {width, height});
}

static void rectWithTexture(WrenVM *vm)
{
  auto x = wrenGetSlotDouble(vm, 1);
  auto y = wrenGetSlotDouble(vm, 2);
  auto z = wrenGetSlotDouble(vm, 3);
  auto width = wrenGetSlotDouble(vm, 4);
  auto height = wrenGetSlotDouble(vm, 5);
  auto *texture = textureInSlot(vm, 6);
  auto texX = wrenGetSlotDouble(vm, 7);
  auto texY = wrenGetSlotDouble(vm, 8);
  auto texW = wrenGetSlotDouble(vm, 9);
  auto texH = wrenGetSlotDouble(vm, 10);
  render::rect(
    {x, y, z}, {width, height},
    *texture, {.pos={texX, texY}, .sz={texW, texH}});
}

static void squareNoTexture(WrenVM *vm)
{
  auto x = wrenGetSlotDouble(vm, 1);
  auto y = wrenGetSlotDouble(vm, 2);
  auto z = wrenGetSlotDouble(vm, 3);
  auto size = wrenGetSlotDouble(vm, 4);
  render::square({x, y, z}, f32(size));
}

static void squareWithTexture(WrenVM *vm)
{
  auto x = wrenGetSlotDouble(vm, 1);
  auto y = wrenGetSlotDouble(vm, 2);
  auto z = wrenGetSlotDouble(vm, 3);
  auto size = wrenGetSlotDouble(vm, 4);
  auto *texture = textureInSlot(vm, 5);
  auto texX = wrenGetSlotDouble(vm, 6);
  auto texY = wrenGetSlotDouble(vm, 7);
  auto texW = wrenGetSlotDouble(vm, 8);
  auto texH = wrenGetSlotDouble(vm, 9);
  render::square(
    {x, y, z}, f32(size),
    *texture, {.pos={texX, texY}, .sz={texW, texH}});
}

static void line(WrenVM *vm)
{
  auto x1 = wrenGetSlotDouble(vm, 1);
  auto y1 = wrenGetSlotDouble(vm, 2);
  auto z1 = wrenGetSlotDouble(vm, 3);
  auto x2 = wrenGetSlotDouble(vm, 4);
  auto y2 = wrenGetSlotDouble(vm, 5);
  auto z2 = wrenGetSlotDouble(vm, 6);
  auto width = wrenGetSlotDouble(vm, 7);
  render::line({x1, y1, z1}, {x2, y2, z2}, f32(width));
}

static void text(WrenVM *vm)
{
  auto x = wrenGetSlotDouble(vm, 1);
  auto y = wrenGetSlotDouble(vm, 2);
  auto z = wrenGetSlotDouble(vm, 3);
  const auto *text = wrenGetSlotString(vm, 4);
  auto height = wrenGetSlotDouble(vm, 5);
  render::text({x, y, z}, text, f32(height));
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature)
{
  BIND(true, "clear(_,_,_)", clear)
  BIND(true, "color(_,_,_,_)", color)
  BIND(true, "rect(_,_,_,_,_)", rectNoTexture)
  BIND(true, "rect(_,_,_,_,_,_,_,_,_,_)", rectWithTexture)
  BIND(true, "square(_,_,_,_)", squareNoTexture)
  BIND(true, "square(_,_,_,_,_,_,_,_,_)", squareWithTexture)
  BIND(true, "line(_,_,_,_,_,_,_)", line)
  BIND(true, "text(_,_,_,_,_)", text)
  return nullptr;
}
