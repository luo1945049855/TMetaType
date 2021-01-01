#include "TMetaType.h"

#include <map>
#include <mutex>
#include <vector>

namespace TMT {
namespace {
using MetaObjectContainer = std::vector<TMT::TMetaObjectPtr>;

inline std::mutex& GetCustomMetaObjectMutex() {
  static std::unique_ptr<std::mutex> g_meta_object_mutex;
  if (!g_meta_object_mutex)
    g_meta_object_mutex = std::make_unique<std::mutex>();
  return *g_meta_object_mutex;
}

inline MetaObjectContainer& GetBuildInMetaObjectContainer() {
  static std::unique_ptr<MetaObjectContainer> g_buildin_meta_objects;
  if (!g_buildin_meta_objects)
    g_buildin_meta_objects = std::make_unique<MetaObjectContainer>();
  return *g_buildin_meta_objects;
}

inline MetaObjectContainer& GetCustomMetaObjectContainer() {
  static std::unique_ptr<MetaObjectContainer> g_meta_objects;
  if (!g_meta_objects)
    g_meta_objects = std::make_unique<MetaObjectContainer>();
  return *g_meta_objects;
}

int32 GetTypeIdByTypeName(const MetaObjectContainer& meta_objects,
                          const std::string& type_name,
                          int32 type_offset) {
  for (size_t i = 0; i < meta_objects.size(); ++i) {
    if (type_name == meta_objects[i]->GetTypeName())
      return i + type_offset;
  }
  return kMetaTypeUnknown;
}

const TMetaObject* GetMetaObjectByTypeId(
    const MetaObjectContainer& meta_objects,
    int32 type_id,
    int32 type_offset) {
  if ((type_id - type_offset) < 0 ||
      (type_id - type_offset) > (int32)meta_objects.size() - 1)
    return nullptr;
  return meta_objects[type_id - type_offset].get();
}
}  // namespace

int32 TMetaType::RegisterMetaType(TMT::TMetaObjectPtr meta_obj,
                                  const TMT::TMetaObject** result_meta_obj) {
  std::unique_lock<std::mutex> lock(GetCustomMetaObjectMutex());
  int32 type_id = GetTypeIdByTypeName(GetCustomMetaObjectContainer(),
                                      meta_obj->GetTypeName(), kMetaTypeUser);
  if (kMetaTypeUnknown != type_id) {
    *result_meta_obj = GetMetaObjectByTypeId(GetCustomMetaObjectContainer(),
                                             type_id, kMetaTypeUser);
    return type_id;
  }

  *result_meta_obj = meta_obj.get();
  MetaObjectContainer& meta_objs = GetCustomMetaObjectContainer();
  meta_objs.push_back(std::move(meta_obj));
  type_id = (kMetaTypeUser + meta_objs.size() - 1);
  return type_id;
}

int32 TMetaType::GetTypeId(const std::string& type_name) {
  int32 type_id = GetTypeIdByTypeName(GetBuildInMetaObjectContainer(),
                                      type_name, kMetaTypeBuildInFirst);
  if (kMetaTypeUnknown != type_id)
    return type_id;

  std::unique_lock<std::mutex> lock(GetCustomMetaObjectMutex());
  return GetTypeIdByTypeName(GetCustomMetaObjectContainer(), type_name,
                             kMetaTypeUser);
}

const TMT::TMetaObject* TMetaType::GetMetaObject(int32 type_id) {
  const TMT::TMetaObject* buildin = GetMetaObjectByTypeId(
      GetBuildInMetaObjectContainer(), type_id, kMetaTypeBuildInFirst);
  if (buildin)
    return buildin;

  std::unique_lock<std::mutex> lock(GetCustomMetaObjectMutex());
  return GetMetaObjectByTypeId(GetCustomMetaObjectContainer(), type_id,
                               kMetaTypeUser);
}

//###########################################################################

#define BUILDIN_TYPE_REGISTER(type, type_id, type_suffix)                      \
  int32 BuildInMetaObj##type_suffix() {                                        \
    TMetaObjectPtr meta_obj = std::make_unique<TMetaObject>(0, std::string()); \
    meta_obj->SetTypeName(#type);                                              \
    meta_obj->SetSizeOf(sizeof(type));                                         \
    meta_obj->SetTypeFlags(TMetaObjectTypeFlagsHelper<type>::Flags);           \
    meta_obj->SetConstructor(TMetaObjectFunctionHelper<type>::Constructor);    \
    meta_obj->SetDestructor(TMetaObjectFunctionHelper<type>::Destructor);      \
    GetBuildInMetaObjectContainer().push_back(std::move(meta_obj));            \
    return type_id;                                                            \
  }                                                                            \
  static const int32 kBuildInMetaTypeInitHelper##type_suffix =                 \
      BuildInMetaObj##type_suffix();

BUILDIN_TYPE_OPERATE(BUILDIN_TYPE_REGISTER)

}  // namespace TMT
