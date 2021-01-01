#ifndef __TVARIANTHELPER_H__
#define __TVARIANTHELPER_H__

#include "TVariant.h"
#include "TTypeTraits.h"

namespace TMT {
template <typename T>
struct TVariantIntegrator {
  static const bool CanUseInternalSpace =
      sizeof(T) <= sizeof(TVariant::Private::Data) &&
      (TTypeTraits<T>::IsRelocatable || std::is_enum<T>::value);
  typedef std::integral_constant<bool, CanUseInternalSpace>
      CanUseInternalSpace_T;
};

template <typename T>
struct TVariantPrivateSharedEx : public TVariant::PrivateShared {
  TVariantPrivateSharedEx() : t_(), TVariant::Private(&t_) {}
  TVariantPrivateSharedEx(const T& t) : t_(t), TVariant::Private(&t_) {}

 private:
  T t_;
};

////////////////////////////////////////////////
// v_construct_helper reference/empty
////////////////////////////////////////////////

template <typename T>
inline void v_construct_helper(TVariant::Private* p,
                               const T& t,
                               std::true_type) {
  new (&p->data) T(t);
  p->is_shared = false;
}

template <typename T>
inline void v_construct_helper(TVariant::Private* p,
                               const T& t,
                               std::false_type) {
  p->data.shared = new TVariantPrivateSharedEx<T>(t);
  p->is_shared = true;
}

template <typename T>
inline void v_construct_helper(TVariant::Private* p, std::true_type) {
  new (&p->data) T();
  p->is_shared = false;
}

template <typename T>
inline void v_construct_helper(TVariant::Private* p, std::false_type) {
  p->data.shared = new TVariantPrivateSharedEx<T>();
  p->is_shared = true;
}

////////////////////////////////////////////////
// v_construct pointer/reference
////////////////////////////////////////////////

template <typename T>
inline void v_construct(TVariant::Private* p, const T& t) {
  v_construct_helper(p, t,
                     typename TVariantIntegrator<T>::CanUseInternalSpace_T());
}

template <typename T>
inline void v_construct(TVariant::Private* p, const void* copy, T* = nullptr) {
  if (copy)
    v_construct<T>(p, *static_cast<const T*>(copy));
  else
    v_construct_helper<T>(p, TVariantIntegrator<T>::CanUseInternalSpace_T());
}

////////////////////////////////////////////////
// v_cast T*/const T*
////////////////////////////////////////////////

template <typename T>
inline T* v_cast(TVariant::Private* p, T* = nullptr) {
  return TVariantIntegrator<T>::CanUseInternalSpace_T()
             ? static_cast<T*>(static_cast<void*>(&p->data))
             : static_cast<T*>(p->data.shared->ptr);
}

template <typename T>
inline const T* v_cast(TVariant::Private* p, T* = nullptr) {
  return TVariantIntegrator<T>::CanUseInternalSpace_T()
             ? static_cast<const T*>(static_cast<const void*>(&p->data))
             : static_cast<const T*>(p->data.shared->ptr);
}

////////////////////////////////////////////////
// v_clear
////////////////////////////////////////////////

template <typename T>
inline void v_clear(TVariant::Private* p, T* = nullptr) {
  if (!TVariantIntegrator<T>::CanUseInternalSpace_T()) {
    // need cast here.
    // because TVariant::PrivateShared doesn't have a virtual destructor.
    delete static_cast<TVariantPrivateSharedEx<T>*>(p->data.shared);
  } else {
    v_cast<T>(p)->~T();
  }
}
}  // namespace TMT

#endif  // __TVARIANTHELPER_H__