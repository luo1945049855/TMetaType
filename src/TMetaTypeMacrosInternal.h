#ifndef __TMETATYPEMACROSINTERNAL_H__
#define __TMETATYPEMACROSINTERNAL_H__

#define TMETA_CLASS_NAME_INTERNAL(class_name) class_name##MetaObject
#define TMETA_CLASS_FUNCTION_PREFIX_INTERNAL(class_name) \
  class_name::TMETA_CLASS_NAME_INTERNAL(class_name)

#define TMETA_CLASS_ACCESSORS_INTERNAL(class_name) \
  static int32 MetaTypeId();                       \
  static const TMT::TMetaObject* MetaObject();     \
  static const char* ClassName();                  \
  virtual const char* GetClassName() const;        \
  virtual int32 GetMetaTypeId() const;             \
  virtual const TMT::TMetaObject* GetMetaObject() const;

#define TMETA_CLASS_INTERNAL(class_name, file, line)                      \
  class TMETA_CLASS_NAME_INTERNAL(class_name) : public TMT::TMetaObject { \
   public:                                                                \
    explicit TMETA_CLASS_NAME_INTERNAL(class_name)()                      \
        : TMT::TMetaObject(line, file) {                                  \
      BuildMetaObject();                                                  \
    }                                                                     \
                                                                          \
   private:                                                               \
    friend class class_name;                                              \
    virtual void BuildMetaObject();                                       \
    static std::atomic_int type_id_;                                      \
    static const TMT::TMetaObject* meta_obj_;                             \
    static const char class_name_[];                                      \
  };

#endif  // __TMETATYPEMACROSINTERNAL_H__