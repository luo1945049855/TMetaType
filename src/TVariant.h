#ifndef __TVARIANT_H__
#define __TVARIANT_H__

#include <atomic>

#include "TTypeTraits.h"

namespace TMT {
class TVariant;
template <typename T>
inline T TVariantCast(const TVariant& v);

class TVariant {
 public:
  struct PrivateShared {
    inline PrivateShared(void* v) : ptr(v), ref(1) {}
    void* ptr;
    std::atomic_int ref;
  };

  struct Private {
    inline Private() noexcept
        : type(kMetaTypeUnknown), is_shared(false), is_null(true) {
      data.ptr = nullptr;
      data.shared = nullptr;
    }
    explicit inline Private(uint32 variant_type) noexcept
        : type(variant_type), is_shared(false), is_null(false) {}

    union Data {
      bool b;
      char c;
      int8 i8;
      uint8 ui8;
      int16 i16;
      uint16 ui16;
      int32 i32;
      uint32 ui32;
      int64 i64;
      uint64 ui64;
      float f;
      double d;
      void* ptr;
      PrivateShared* shared;
    } data;
    uint32 type : 30;
    uint32 is_shared : 1;
    uint32 is_null : 1;
  };

  TVariant() noexcept;
  TVariant(bool b);
  TVariant(int8 i8);
  TVariant(uint8 ui8);
  TVariant(int16 i16);
  TVariant(uint16 ui16);
  TVariant(int32 i32);
  TVariant(uint32 ui32);
  TVariant(int64 i64);
  TVariant(uint64 ui64);
  TVariant(float f);
  TVariant(double d);
  TVariant(char c);
  TVariant(int32 type_id, const void* copy);
  TVariant(const TVariant& other);
  ~TVariant();

  template <typename T>
  inline T Value() const {
    return TVariantCast<T>(*this);
  }

  int32 TypeId() const;
  const void* ConstData() const;

 private:
  void Create(int32 type_id, const void* copy);

 private:
  Private private_;
};

template <typename T, typename Enable = void>
struct TVariantValueHelper {
  static T MetaType(const TVariant& v) { return T(); }
};

template <typename T>
struct TVariantValueHelper<
    T,
    typename std::enable_if<std::is_arithmetic<T>::value>::type> {
  static T MetaType(const TVariant& v) {
    switch (v.TypeId()) {
      case kMetaTypeBool:
        return static_cast<T>(*static_cast<const bool*>(v.ConstData()));
      case kMetaTypeInt8:
        return static_cast<T>(*static_cast<const int8*>(v.ConstData()));
      case kMetaTypeUInt8:
        return static_cast<T>(*static_cast<const uint8*>(v.ConstData()));
      case kMetaTypeInt16:
        return static_cast<T>(*static_cast<const int16*>(v.ConstData()));
      case kMetaTypeUInt16:
        return static_cast<T>(*static_cast<const uint16*>(v.ConstData()));
      case kMetaTypeInt32:
        return static_cast<T>(*static_cast<const int32*>(v.ConstData()));
      case kMetaTypeUInt32:
        return static_cast<T>(*static_cast<const uint32*>(v.ConstData()));
      case kMetaTypeInt64:
        return static_cast<T>(*static_cast<const int64*>(v.ConstData()));
      case kMetaTypeUInt64:
        return static_cast<T>(*static_cast<const uint64*>(v.ConstData()));
      case kMetaTypeFloat:
        return static_cast<T>(*static_cast<const float*>(v.ConstData()));
      case kMetaTypeDouble:
        return static_cast<T>(*static_cast<const double*>(v.ConstData()));
      case kMetaTypeChar:
        return static_cast<T>(*static_cast<const char*>(v.ConstData()));
    }
    return T();
  }
};

template <typename T>
inline T TVariantCast(const TVariant& v) {
  const int32 type_id = TMetaTypeId<T>::MetaTypeId();
  if (type_id == v.TypeId())
    return *static_cast<const T*>(v.ConstData());
  return TVariantValueHelper<T>::MetaType(v);
}

}  // namespace TMT

#endif  // __TVARIANT_H__
