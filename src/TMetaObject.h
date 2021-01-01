#ifndef __TMETACLASS_H__
#define __TMETACLASS_H__

#include <memory>
#include <string>

#include "TTypeTraits.h"
#include "TMetaProperty.h"

namespace TMT {
class TMetaObject {
 public:
  typedef void* (*Constructor)(void*, const void*);
  typedef void (*Destructor)(void*);

  TMetaObject(uint32 line, std::string file);
  virtual ~TMetaObject();

  const std::string& GetFileName() const { return file_name_; }
  uint32 GetLineNum() const { return line_; }

  void SetTypeName(const std::string& type_name);
  const std::string& GetTypeName() const { return type_name_; }

  void SetSizeOf(uint32 sizeof_type);
  uint32 GetSizeOf() const { return sizeof_type_; }

  void SetParentMetaObject(const TMetaObject* parent_meta_obj);
  const TMetaObject* GetParentMetaObject() const { return parent_meta_obj_; }

  void SetTypeFlags(uint32 type_flags);
  uint32 GetTypeFlags() const { return type_flags_; }

  void SetConstructor(Constructor constructor);
  Constructor GetConstructor() const { return constructor_; }

  void SetDestructor(Destructor destructor);
  Destructor GetDestructor() const { return destructor_; }

  void AddProperty(TMetaPropertyPtr&& meta_prop);
  const TMetaProperty* GetProperty(const std::string& prop_name) const;
  const TMetaPropertyContainter& GetAllProperties() const;

  inline void* Construct(void* where, const void* copy) const {
    if (!constructor_)
      return nullptr;
    return constructor_(where, copy);
  }

  inline void Destruct(void* t) const {
    if (!t)
      return;
    if (!destructor_)
      return;
    destructor_(t);
  }

 private:
  const uint32 line_;
  const std::string file_name_;
  const TMetaObject* parent_meta_obj_;

  uint32 sizeof_type_;
  uint32 type_flags_;

  std::string type_name_;

  Constructor constructor_;
  Destructor destructor_;

  TMetaPropertyContainter properties_;
};
typedef std::unique_ptr<TMetaObject> TMetaObjectPtr;
}  // namespace TMT

#endif  // __TMETACLASS_H__