#include "TMetaObject.h"

namespace TMT {
TMetaObject::TMetaObject(uint32 line, std::string file)
    : line_(line),
      file_name_(file),
      parent_meta_obj_(nullptr),
      sizeof_type_(0),
      type_flags_(0),
      constructor_(nullptr),
      destructor_(nullptr) {}

TMetaObject::~TMetaObject() {}

void TMetaObject::SetTypeName(const std::string& type_name) {
  type_name_ = type_name;
}

void TMetaObject::SetSizeOf(uint32 sizeof_type) {
  sizeof_type_ = sizeof_type;
}

void TMetaObject::SetParentMetaObject(const TMetaObject* parent_meta_obj) {
  parent_meta_obj_ = parent_meta_obj;
}

void TMetaObject::SetTypeFlags(uint32 type_flags) {
  type_flags_ = type_flags;
}

void TMetaObject::SetConstructor(Constructor constructor) {
  constructor_ = constructor;
}

void TMetaObject::SetDestructor(Destructor destructor) {
  destructor_ = destructor;
}

void TMetaObject::AddProperty(TMetaPropertyPtr&& meta_prop) {
  properties_.push_back(std::move(meta_prop));
}

const TMetaProperty* TMetaObject::GetProperty(
    const std::string& prop_name) const {
  TMetaPropertyContainter::const_iterator it_prop =
      std::find_if(properties_.cbegin(), properties_.cend(),
                   [&prop_name](const TMetaPropertyPtr& meta_prop) -> bool {
                     return meta_prop->GetPropertyName() == prop_name;
                   });
  if (it_prop != properties_.end())
    return it_prop->get();
  return nullptr;
}

const TMetaPropertyContainter& TMetaObject::GetAllProperties() const {
  return properties_;
}
}  // namespace TMT
