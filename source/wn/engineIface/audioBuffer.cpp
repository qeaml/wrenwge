#include "../engineIface.hpp"

using namespace nwge;

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindAudioBufferMethod(bool isStatic, const char *signature)
{
  return nullptr;
}

static void audioBufferAllocate(WrenVM *vm)
{
  auto *audioBuffer = wrenSetSlotNewForeign(vm, 0, 0, sizeof(audio::Buffer));
  new(audioBuffer) audio::Buffer();
}

static void audioBufferFinalize(void *data)
{
  auto *audioBuffer = reinterpret_cast<audio::Buffer*>(data);
  audioBuffer->~Buffer();
}

WrenForeignClassMethods bindAudioBufferClass()
{
  return {audioBufferAllocate, audioBufferFinalize};
}
