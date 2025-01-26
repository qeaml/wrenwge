#include "../engineIface.hpp"
#include <nwge/dialog.hpp>
#include <nwge/json/parse.hpp>

using namespace nwge;

static void json2wren(WrenVM *vm, int slot, const json::Value &value)
{
  if(value.isString()) {
    const auto &str = value.string();
    wrenSetSlotBytes(vm, slot, str.data(), str.size());
    return;
  }
  if(value.isNumber()) {
    wrenSetSlotDouble(vm, slot, value.number());
    return;
  }
  if(value.isObject()) {
    wrenSetSlotNewMap(vm, slot);
    wrenEnsureSlots(vm, slot+3);
    for(const auto &pair: value.object().pairs()) {
      wrenSetSlotBytes(vm, slot+1, pair.key.data(), pair.key.size());
      json2wren(vm, slot+2, pair.value);
      wrenSetMapValue(vm, slot, slot+1, slot+2);
    }
    return;
  }
  if(value.isArray()) {
    wrenSetSlotNewList(vm, slot);
    wrenEnsureSlots(vm, slot+2);
    for(const auto &elem: value.array()) {
      json2wren(vm, slot+1, elem);
      wrenInsertInList(vm, slot, -1, slot+1);
    }
    return;
  }
  if(value.isBoolean()) {
    wrenSetSlotBool(vm, slot, value.boolean());
    return;
  }
  if(value.isNull()) {
    wrenSetSlotNull(vm, slot);
    return;
  }
}

static void parseFromString(WrenVM *vm)
{
  int length;
  ConstCStr ptr = wrenGetSlotBytes(vm, 1, &length);
  StringView view{ptr, static_cast<usize>(length)};

  auto res = json::parse(view);
  if(res.error) {
    dialog::error("JSON error"_sv,
      "{}", res.error.message());
    wrenSetSlotNull(vm, 0);
    return;
  }
  json2wren(vm, 0, *res.value);
}

static void parse(WrenVM *vm)
{
  switch(wrenGetSlotType(vm, 1)) {
  case WREN_TYPE_STRING:
    parseFromString(vm);
    return;

  default:
    wrenSetSlotNull(vm, 0);
    return;
  }
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindJsonMethod(bool isStatic, const char *signature)
{
  BIND(true, "parse(_)", parse)
  return nullptr;
}
