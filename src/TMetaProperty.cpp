#include "TMetaProperty.h"

namespace TMT {
TMetaProperty::TMetaProperty() : prop_flags_(0) {}

TMetaProperty::~TMetaProperty() {}

void TMetaProperty::SetPropertyFlags(uint32 flags) {
  prop_flags_ = flags;
}

void TMetaProperty::SetPropertyName(const std::string& prop_name) {
  prop_name_ = prop_name;
}

void TMetaProperty::SetPropertyType(const std::string& prop_type) {
  prop_type_ = prop_type;
}
}  // namespace TMT