#pragma once

#include <memory>

#include "TMetaObject.h"
#include "TMetaTypeHeaderMacros.h"
#include "TMetaTypeImplMacros.h"

namespace TMT {
class TMetaType {
 public:
  template <typename MetaObjectType>
  friend int32 RegisterMetaObject(const TMT::TMetaObject** result_meta_obj);

  static int32 GetTypeId(const std::string& type_name);
  static const TMT::TMetaObject* GetMetaObject(int32 type_id);

 protected:
  TMetaType() = delete;
  ~TMetaType() = delete;

 private:
  static int32 RegisterMetaType(TMT::TMetaObjectPtr meta_obj,
                                const TMT::TMetaObject** result_meta_obj);
};

template <typename MetaObjectType>
int32 RegisterMetaObject(const TMT::TMetaObject** result_meta_obj) {
  std::unique_ptr<MetaObjectType> meta_obj = std::make_unique<MetaObjectType>();
  return TMetaType::RegisterMetaType(std::move(meta_obj), result_meta_obj);
}
}  // namespace TMT
