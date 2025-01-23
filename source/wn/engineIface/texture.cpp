#include "../engineIface.hpp"
#include "wren.h"
#include <nwge/render/Texture.hpp>

using namespace nwge;

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindTextureMethod(bool isStatic, const char *signature)
{
  return nullptr;
}

static void textureAllocate(WrenVM *vm)
{
  auto **texture = (render::Texture**)wrenSetSlotNewForeign(vm, 0, 0, sizeof(render::Texture*));
  *texture = new render::Texture();
}

static void textureFinalize(void *data)
{
  auto *texture = (render::Texture**)data;
  delete *texture;
}

WrenForeignClassMethods bindTextureClass()
{
  return {textureAllocate, textureFinalize};
}
