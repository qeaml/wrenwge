#include "../engineIface.hpp"
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
  auto *texture = wrenSetSlotNewForeign(vm, 0, 0, sizeof(render::Texture));
  new(texture) render::Texture();
}

static void textureFinalize(void *data)
{
  auto *texture = reinterpret_cast<render::Texture*>(data);
  texture->~Texture();
}

WrenForeignClassMethods bindTextureClass()
{
  return {textureAllocate, textureFinalize};
}
