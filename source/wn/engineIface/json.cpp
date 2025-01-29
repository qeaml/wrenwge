#include "../engineIface.hpp"
#include <cstring>
#include <nwge/dialog.hpp>
#include <nwge/json/builder.hpp>
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

static json::Value wren2json(WrenVM *vm, int slot)
{
  switch(wrenGetSlotType(vm, slot)) {
  case WREN_TYPE_BOOL:
    return wrenGetSlotBool(vm, slot);

  case WREN_TYPE_NUM:
    return wrenGetSlotDouble(vm, slot);

  case WREN_TYPE_LIST: {
    json::ArrayBuilder builder;
    int count = wrenGetListCount(vm, slot);
    wrenEnsureSlots(vm, slot+2);
    for(int i = 0; i < count; ++i) {
      wrenGetListElement(vm, slot, i, slot+1);
      builder.add(wren2json(vm, slot+1));
    }
    return builder.finish();
  }

  case WREN_TYPE_MAP: {
    json::ObjectBuilder builder;
    wrenEnsureSlots(vm, slot+3);
    auto *iter = wrenIterateMap(vm, slot);
    while(wrenNextInMap(iter, slot+1, slot+2) != 0) {
      if(wrenGetSlotType(vm, slot+1) != WREN_TYPE_STRING) {
        continue;
      }
      builder.set(wrenGetSlotString(vm, slot+1), wren2json(vm, slot+2));
    }
    wrenFreeIterator(iter);
    return builder.finish();
  }

  case WREN_TYPE_NULL:
    return nullptr;

  case WREN_TYPE_STRING: {
    int length;
    ConstCStr ptr = wrenGetSlotBytes(vm, slot, &length);
    return StringView{ptr, static_cast<usize>(length)};
  }
  }

  NWGE_UNREACHABLE("Invalid Wren object type");
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

static void encode(WrenVM *vm)
{
  auto value = wren2json(vm, 1);
  auto str = json::encode(value);
  wrenSetSlotBytes(vm, 0, str.data(), str.size());
}

#define BIND(methodStatic, methodSignature, method) \
  if(isStatic == (methodStatic) && strcmp(signature, methodSignature) == 0) { \
    return &(method); \
  }

WrenForeignMethodFn bindJsonMethod(bool isStatic, const char *signature)
{
  BIND(true, "parse(_)", parse)
  BIND(true, "encode(_)", encode)
  return nullptr;
}
