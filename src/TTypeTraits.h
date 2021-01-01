#ifndef __TTYPETRAITS_H__
#define __TTYPETRAITS_H__

#include <stdint.h>
#include <type_traits>

typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

namespace TMT {
template <typename T>
static constexpr bool fIsTrivial() {
  return std::is_trivial<T>::value;
}

template <typename T>
static constexpr bool fIsRelocatable() {
  return std::is_trivially_copyable<T>::value &&
         std::is_trivially_destructible<T>::value;
}

#define BUILDIN_TYPE_DEFINE(type, type_id, type_suffix)               \
  enum {                                                              \
    kMetaType##type_suffix = type_id,                                 \
  };                                                                  \
  template <>                                                         \
  struct TMetaTypeId<type> {                                          \
    enum {                                                            \
      BuildIn = 1,                                                    \
      Defined = 1,                                                    \
    };                                                                \
    static int32 MetaTypeId() { return TMT::kMetaType##type_suffix; } \
  };

#define BUILDIN_TYPE_OPERATE(operator)                                      \
  operator(bool, 1, Bool) operator(int8, 2, Int8) operator(uint8, 3, UInt8) \
  operator(int16, 4, Int16)                                                 \
  operator(uint16, 5, UInt16)                                               \
  operator(int32, 6, Int32)                                                 \
  operator(uint32, 7, UInt32)                                               \
  operator(int64, 8, Int64)                                                 \
  operator(uint64, 9, UInt64)                                               \
  operator(float, 10, Float)                                                \
  operator(double, 11, Double)                                              \
  operator(char, 12, Char)

enum {
  kMetaTypeUnknown = 0,
  kMetaTypeBuildInFirst = 1,
  kMetaTypeBuildInLast = 1023,
  kMetaTypeUser = 1024,
};

template <typename T>
struct TMetaTypeId {
  enum {
    BuildIn = 0,
    Defined = 0,
  };
  static int32 MetaTypeId() { return TMT::kMetaTypeUnknown; }
};
BUILDIN_TYPE_OPERATE(BUILDIN_TYPE_DEFINE)

template <typename T>
static constexpr bool fIsBuildIn() {
  return TMetaTypeId<T>::BuildIn;
}

template <typename T>
struct TTypeTraits {
  enum {
    // don't require every enum to be marked manually
    IsSpecialized = std::is_enum<T>::value,
    IsPointer = false,
    IsIntegral = std::is_integral<T>::value,
    IsComplex = !fIsTrivial<T>(),
    IsStatic = true,
    IsRelocatable = fIsRelocatable<T>(),
    IsLarge = sizeof(T) > sizeof(void*),
    SizeOf = sizeof(T),
  };
};

enum TypeFlags {
  MovableType = 0x0001,
  NeedsConstruction = 0x0001 << 1,
  NeedsDestruction = 0x0001 << 2,
  IsEnumeration = 0x0001 << 3,
};

template <typename T>
struct TMetaObjectTypeFlagsHelper {
  enum {
    Flags = (TTypeTraits<T>::IsRelocatable ? MovableType : 0) |
            (TTypeTraits<T>::IsComplex ? NeedsConstruction : 0) |
            (TTypeTraits<T>::IsComplex ? NeedsDestruction : 0) |
            (std::is_enum<T>::value ? IsEnumeration : 0)
  };
};

template <typename T, bool IsComplex = TTypeTraits<T>::IsComplex>
struct TMetaObjectFunctionHelper {
  static void* Constructor(void* where, const void* t);
  static void Destructor(void* t);
};

template <typename T>
struct TMetaObjectFunctionHelper<T, true> {
  static void* Constructor(void* where, const void* t) {
    if (t)
      return new (where) T(*static_cast<const T*>(t));
    else
      return new (where) T();
  }
  static void Destructor(void* t) { static_cast<T*>(t)->~T(); }
};

template <typename T>
struct TMetaObjectFunctionHelper<T, false> {
  static void* Constructor(void* where, const void* t) {
    if (t)
      return new (where) T(*static_cast<const T*>(t));
    else
      return new (where) T();
  }
  static void Destructor(void* t) {}
};
}  // namespace TMT

#endif  // __TTYPETRAITS_H__
