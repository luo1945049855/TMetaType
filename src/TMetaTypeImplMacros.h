#ifndef __TMETATYPEIMPLMACROS_H__
#define __TMETATYPEIMPLMACROS_H__

#define BEGIN_META_CLASS(class_name)                                           \
  const char* class_name::GetClassName() const {                               \
    return class_name::ClassName();                                            \
  }                                                                            \
  const TMT::TMetaObject* class_name::GetMetaObject() const {                  \
    return class_name::MetaObject();                                           \
  }                                                                            \
  int32 class_name::GetMetaTypeId() const { return class_name::MetaTypeId(); } \
                                                                               \
  std::atomic_int TMETA_CLASS_FUNCTION_PREFIX_INTERNAL(                        \
      class_name)::type_id_ = {TMT::kMetaTypeUnknown};                         \
  const TMT::TMetaObject* TMETA_CLASS_FUNCTION_PREFIX_INTERNAL(                \
      class_name)::meta_obj_ = nullptr;                                        \
  const char TMETA_CLASS_FUNCTION_PREFIX_INTERNAL(class_name)::class_name_[] = \
      #class_name;                                                             \
                                                                               \
  int32 class_name::MetaTypeId() {                                             \
    if (TMT::kMetaTypeUnknown ==                                               \
        TMETA_CLASS_NAME_INTERNAL(class_name)::type_id_.load(                  \
            std::memory_order_acquire)) {                                      \
      const int32 new_type_id =                                                \
          TMT::RegisterMetaObject<TMETA_CLASS_NAME_INTERNAL(class_name)>(      \
              &TMETA_CLASS_NAME_INTERNAL(class_name)::meta_obj_);              \
      TMETA_CLASS_NAME_INTERNAL(class_name)::type_id_.store(                   \
          new_type_id, std::memory_order_release);                             \
    }                                                                          \
    return TMETA_CLASS_NAME_INTERNAL(class_name)::type_id_;                    \
  }                                                                            \
  const TMT::TMetaObject* class_name::MetaObject() {                           \
    return TMETA_CLASS_NAME_INTERNAL(class_name)::meta_obj_;                   \
  }                                                                            \
  const char* class_name::ClassName() {                                        \
    return TMETA_CLASS_NAME_INTERNAL(class_name)::class_name_;                 \
  }                                                                            \
                                                                               \
  template <>                                                                  \
  struct TMT::TMetaTypeId<class_name> {                                        \
    enum {                                                                     \
      BuildIn = 0,                                                             \
      Defined = 1,                                                             \
    };                                                                         \
    static int32 MetaTypeId() { return class_name::MetaTypeId(); }             \
  };                                                                           \
  static const int32 kMetaTypeInitHelper##class_name =                         \
      TMT::TMetaTypeId<class_name>::MetaTypeId();                              \
                                                                               \
  void TMETA_CLASS_FUNCTION_PREFIX_INTERNAL(class_name)::BuildMetaObject() {   \
    SetTypeName(std::string(#class_name));                                     \
    SetSizeOf(sizeof(class_name));                                             \
    SetTypeFlags(TMT::TMetaObjectTypeFlagsHelper<class_name>::Flags);          \
    SetConstructor(TMT::TMetaObjectFunctionHelper<class_name>::Constructor);   \
    SetDestructor(TMT::TMetaObjectFunctionHelper<class_name>::Destructor);

#define END_META_CLASS() }

#define TMETA_CLASS_PARENT(parent_class_name) \
  SetParentMetaObject(parent_class_name::MetaObject());

#define ADD_META_PROPERTY(class_name, property_type, property_name)          \
  TMT::TMetaPropertyPtr meta_prop =                                          \
      TMT::MakeMetaProperty<class_name, property_type,                       \
                            decltype(&class_name::Get##property_name),       \
                            decltype(&class_name::Set##property_name)>(      \
          &class_name::Get##property_name, &class_name::Set##property_name); \
  meta_prop->SetPropertyType(#property_type);                                \
  meta_prop->SetPropertyName(#property_name);                                \
  AddProperty(std::move(meta_prop));

#define ADD_META_PROPERTY_READONLY(class_name, property_type, property_name) \
  TMT::TMetaPropertyPtr meta_prop =                                          \
      TMT::MakeMetaProperty<class_name, property_type,                       \
                            decltype(&class_name::Get##property_name)>(      \
          &class_name::Get##property_name);                                  \
  meta_prop->SetPropertyType(#property_type);                                \
  meta_prop->SetPropertyName(#property_name);                                \
  AddProperty(std::move(meta_prop));

#endif  // __TMETATYPEIMPLMACROS_H__