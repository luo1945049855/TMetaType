#ifndef __TMETAPROPERTY_H__
#define __TMETAPROPERTY_H__

#include <vector>
#include <string>
#include <memory>

#include "TVariant.h"

namespace TMT {
class TMetaProperty {
 public:
  TMetaProperty();
  virtual ~TMetaProperty();

  void SetPropertyFlags(uint32 flags);
  uint32 GetPropertyFlags() const { return prop_flags_; }

  void SetPropertyName(const std::string& prop_name);
  const std::string& GetPropertyName() const { return prop_name_; }

  void SetPropertyType(const std::string& prop_type);
  const std::string& GetPropertyType() const { return prop_type_; }

  virtual TVariant GetPropValue(void* obj) const = 0;
  virtual void SetPropValue(void* obj, const TVariant& v) = 0;

 private:
  uint32 prop_flags_;
  std::string prop_name_;
  std::string prop_type_;
};
typedef std::unique_ptr<TMetaProperty> TMetaPropertyPtr;
typedef std::vector<TMetaPropertyPtr> TMetaPropertyContainter;

template <typename ClassType, typename PropType, typename GetterType>
class TMetaPropertyReadonly : public TMetaProperty {
 public:
  TMetaPropertyReadonly(GetterType getter) : getter_(getter) {}
  virtual ~TMetaPropertyReadonly() {}

 protected:
  TVariant GetPropValue(void* obj) const override {
    const PropType& value = (static_cast<ClassType*>(obj)->*getter_)();
    return TVariant(TMT::TMetaTypeId<PropType>::MetaTypeId(), &value);
  }

  void SetPropValue(void* obj, const TVariant& v) override {}

 private:
  GetterType getter_;
};

template <typename ClassType,
          typename PropType,
          typename GetterType,
          typename SetterType>
class TMetaPropertyReadWrite
    : public TMetaPropertyReadonly<ClassType, PropType, GetterType> {
 public:
  TMetaPropertyReadWrite(GetterType getter, SetterType setter)
      : TMetaPropertyReadonly<ClassType, PropType, GetterType>(getter),
        setter_(setter) {}
  virtual ~TMetaPropertyReadWrite() {}

 protected:
  void SetPropValue(void* obj, const TVariant& v) final {
    if (v.TypeId() != TMT::TMetaTypeId<PropType>::MetaTypeId())
      return;
    (static_cast<ClassType*>(obj)->*setter_)(v.Value<PropType>());
  }

 private:
  SetterType setter_;
};

template <typename ClassType, typename PropType, typename GetterType>
TMetaPropertyPtr MakeMetaProperty(GetterType getter) {
  return std::make_unique<
      TMetaPropertyReadonly<ClassType, PropType, GetterType>>(getter);
}

template <typename ClassType,
          typename PropType,
          typename GetterType,
          typename SetterType>
TMetaPropertyPtr MakeMetaProperty(GetterType getter, SetterType setter) {
  return std::make_unique<
      TMetaPropertyReadWrite<ClassType, PropType, GetterType, SetterType>>(
      getter, setter);
}
}  // namespace TMT

#endif  // __TMETAPROPERTY_H__