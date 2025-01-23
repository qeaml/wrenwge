#include "../engineIface.hpp"
#include <nwge/render/draw.hpp>

using namespace nwge;

static void listToVec2(WrenVM *vm, int listSlot, glm::vec2 &out, int tempSlot)
{
  int elems = wrenGetListCount(vm, listSlot);
  if(elems == 0) {
    return;
  }
  wrenGetListElement(vm, listSlot, 0, tempSlot);
  out.x = wrenGetSlotDouble(vm, tempSlot);
  if(elems < 2) {
    out.y = out.x;
    return;
  }
  wrenGetListElement(vm, listSlot, 1, tempSlot);
  out.y = wrenGetSlotDouble(vm, tempSlot);
}

static void listToVec3(WrenVM *vm, int listSlot, glm::vec3 &out, int tempSlot)
{
  int elems = wrenGetListCount(vm, listSlot);
  if(elems == 0) {
    return;
  }
  wrenGetListElement(vm, listSlot, 0, tempSlot);
  out.x = wrenGetSlotDouble(vm, tempSlot);
  if(elems < 3) {
    out.y = out.x;
    out.z = out.x;
    return;
  }
  wrenGetListElement(vm, listSlot, 1, tempSlot);
  out.y = wrenGetSlotDouble(vm, tempSlot);
  wrenGetListElement(vm, listSlot, 2, tempSlot);
  out.z = wrenGetSlotDouble(vm, tempSlot);
}

static void listToVec4(WrenVM *vm, int listSlot, glm::vec4 &out, int tempSlot)
{
  int elems = wrenGetListCount(vm, listSlot);
  if(elems == 0) {
    return;
  }
  wrenGetListElement(vm, listSlot, 0, tempSlot);
  out.x = wrenGetSlotDouble(vm, tempSlot);
  if(elems < 3) {
    out.y = out.x;
    out.z = out.x;
    return;
  }
  wrenGetListElement(vm, listSlot, 1, tempSlot);
  out.y = wrenGetSlotDouble(vm, tempSlot);
  wrenGetListElement(vm, listSlot, 2, tempSlot);
  out.z = wrenGetSlotDouble(vm, tempSlot);
  if(elems < 4) {
    return;
  }
  wrenGetListElement(vm, listSlot, 3, tempSlot);
  out.w = wrenGetSlotDouble(vm, tempSlot);
}

static void listToTexCoord(WrenVM *vm, int listSlot, render::TexCoord &out, int tempSlot)
{
  int elems = wrenGetListCount(vm, listSlot);
  if(elems < 4) {
    return;
  }
  wrenGetListElement(vm, listSlot, 0, tempSlot);
  out.pos.x = wrenGetSlotDouble(vm, tempSlot);
  wrenGetListElement(vm, listSlot, 1, tempSlot);
  out.pos.y = wrenGetSlotDouble(vm, tempSlot);
  wrenGetListElement(vm, listSlot, 2, tempSlot);
  out.sz.x = wrenGetSlotDouble(vm, tempSlot);
  wrenGetListElement(vm, listSlot, 3, tempSlot);
  out.sz.y = wrenGetSlotDouble(vm, tempSlot);
}

static void clear(WrenVM *vm)
{
  wrenEnsureSlots(vm, 3);
  glm::vec3 color{0.3, 0.3, 0.3};
  listToVec3(vm, 1, color, 2);
  render::clear(color);
}

static void color(WrenVM *vm)
{
  glm::vec4 color{1, 1, 1, 1};
  wrenEnsureSlots(vm, 3);
  listToVec4(vm, 1, color, 2);
  render::color(color);
}

static void rectNoTexture(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> pos list - 3 elements
  slot 2 -> width list - 2 elements
  slot 3 -> temporary slot
    = 4 total
  */
  wrenEnsureSlots(vm, 4);
  glm::vec3 pos{0, 0, 0};
  glm::vec2 size{1, 1};
  listToVec3(vm, 1, pos, 3);
  listToVec2(vm, 2, size, 3);
  render::rect(pos, size);
}

static void rectWithTexture(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> pos list - 3 elements
  slot 2 -> size list - 2 elements
  slot 3 -> texture object
  slot 4 -> tex pos list - 4 elements
  slot 5 -> temporary slot
    = 6 total
  */
  wrenEnsureSlots(vm, 6);
  glm::vec3 pos{0, 0, 0};
  glm::vec2 size{1, 1};
  listToVec3(vm, 1, pos, 5);
  listToVec2(vm, 2, size, 5);
  auto *texture = textureInSlot(vm, 3);
  render::TexCoord texCoord{.pos={0, 0}, .sz={1, 1}};
  listToTexCoord(vm, 4, texCoord, 5);
  render::rect(pos, size, *texture, texCoord);
}

static void squareNoTexture(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> pos list - 3 elements
  slot 2 -> size
  slot 3 -> temporary slot
    = 4 total
  */
  wrenEnsureSlots(vm, 4);
  glm::vec3 pos{0, 0, 0};
  listToVec3(vm, 1, pos, 3);
  auto size = wrenGetSlotDouble(vm, 2);
  render::square(pos, f32(size));
}

static void squareWithTexture(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> pos list - 3 elements
  slot 2 -> size
  slot 3 -> texture object
  slot 4 -> tex coord list - 4 elements
  slot 5 -> temporary slot
    = 6 total
  */
  wrenEnsureSlots(vm, 6);
  glm::vec3 pos{0, 0, 0};
  listToVec3(vm, 1, pos, 5);
  auto size = wrenGetSlotDouble(vm, 2);
  auto *texture = textureInSlot(vm, 3);
  render::TexCoord texCoord{.pos={0, 0}, .sz={1, 1}};
  listToTexCoord(vm, 4, texCoord, 5);
  render::square(pos, f32(size), *texture, texCoord);
}

static void line(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> start pos list - 3 elements
  slot 2 -> end pos list - 3 elements
  slot 3 -> width
  slot 4 -> temporary slot
    = 5 total
  */
  wrenEnsureSlots(vm, 5);
  glm::vec3 start{0, 0, 0};
  glm::vec3 end{0, 0, 0};
  listToVec3(vm, 1, start, 3);
  listToVec3(vm, 2, end, 3);
  auto width = wrenGetSlotDouble(vm, 3);
  render::line(start, end, f32(width));
}

static void text(WrenVM *vm)
{
  /*
  slot 0 -> pointer to Render class
  slot 1 -> pos list - 3 elements
  slot 2 -> text
  slot 3 -> height
  slot 4 -> temporary slot
    = 5 total
  */
  wrenEnsureSlots(vm, 5);
  glm::vec3 pos{0, 0, 0};
  listToVec3(vm, 1, pos, 4);
  const auto *text = wrenGetSlotString(vm, 2);
  auto height = wrenGetSlotDouble(vm, 3);
  render::text(pos, text, f32(height));
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindRenderMethod(bool isStatic, const char *signature)
{
  BIND(true, "clear(_)", clear)
  BIND(true, "color(_)", color)
  BIND(true, "rect(_,_)", rectNoTexture)
  BIND(true, "rect(_,_,_,_)", rectWithTexture)
  BIND(true, "square(_,_)", squareNoTexture)
  BIND(true, "square(_,_,_,_)", squareWithTexture)
  BIND(true, "line(_,_,_)", line)
  BIND(true, "text(_,_,_)", text)
  return nullptr;
}
