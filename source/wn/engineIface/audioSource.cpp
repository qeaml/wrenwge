#include "../engineIface.hpp"

using namespace nwge;

static void buffer(WrenVM *vm)
{
  auto *source = audioSourceInSlot(vm, 0);
  auto *buffer = audioBufferInSlot(vm, 1);
  source->buffer(*buffer);
}

static void play(WrenVM *vm)
{
  auto *source = audioSourceInSlot(vm, 0);
  source->play();
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindAudioSourceMethod(bool isStatic, const char *signature)
{
  BIND(false, "buffer(_)", buffer);
  BIND(false, "play()", play);
  return nullptr;
}

static void audioSourceAllocate(WrenVM *vm)
{
  auto *audioSource = wrenSetSlotNewForeign(vm, 0, 0, sizeof(audio::Source));
  new(audioSource) audio::Source();
}

static void audioSourceFinalize(void *data)
{
  auto *audioSource = reinterpret_cast<audio::Source*>(data);
  audioSource->~Source();
}

WrenForeignClassMethods bindAudioSourceClass()
{
  return {audioSourceAllocate, audioSourceFinalize};
}
