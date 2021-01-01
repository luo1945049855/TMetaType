#include "TVariant.h"
#include "TMetaType.h"

namespace TMT {

namespace {
void TVariantConstruct(TVariant::Private* p, const void* copy) {
  const TMetaObject* meta_obj = TMetaType::GetMetaObject(p->type);
  if (!meta_obj)
    return;

  // this logic should match with TVariantIntegrator::CanUseInternalSpace
  const uint32 size = meta_obj->GetSizeOf();
  if (size <= sizeof(p->data) &&
      (meta_obj->GetTypeFlags() & (MovableType | IsEnumeration))) {
    meta_obj->Construct(&(p->data.ptr), copy);
    p->is_null = p->data.ptr == nullptr;
    p->is_shared = false;
  } else {
    // ??Private::Data contains int64, and long double is the biggest
    // standard type.

    // A + ((A * B - B) % B) = (mB + n) + ((mB+n) * (B-1) % B)
    // 0 <= m
    // 0 <= n < B

    const size_t max_alignment =
        std::max(alignof(TVariant::Private::Data), alignof(long double));
    const size_t size_shared = sizeof(TVariant::PrivateShared);
    const size_t offset =
        size_shared +
        ((size_shared * max_alignment - size_shared) % max_alignment);
    void* data = operator new(offset + size);
    void* ptr = static_cast<char*>(data) + offset;
    meta_obj->Construct(ptr, copy);
    p->is_null = ptr == nullptr;
    p->is_shared = true;
    p->data.shared = new (data) TVariant::PrivateShared(ptr);
  }
}

void TVariantClear(TVariant::Private* p) {
  const TMetaObject* meta_obj = TMetaType::GetMetaObject(p->type);
  if (!meta_obj)
    return;

  if (!p->is_shared) {
    meta_obj->Destruct(&p->data.ptr);
  } else {
    meta_obj->Destruct(p->data.shared->ptr);
    p->data.shared->~PrivateShared();
    operator delete(p->data.shared);
  }
}
}  // namespace

TVariant::TVariant() noexcept {}

TVariant::~TVariant() {
  if ((private_.is_shared && 0 == --private_.data.shared->ref) ||
      (!private_.is_shared && private_.type != kMetaTypeUnknown)) {
    TVariantClear(&private_);
  }
}

TVariant::TVariant(bool b) : private_(kMetaTypeBool) {
  private_.data.b = b;
}

TVariant::TVariant(int8 i8) : private_(kMetaTypeInt8) {
  private_.data.i8 = i8;
}

TVariant::TVariant(uint8 ui8) : private_(kMetaTypeUInt8) {
  private_.data.ui8 = ui8;
}

TVariant::TVariant(int16 i16) : private_(kMetaTypeInt16) {
  private_.data.i16 = i16;
}

TVariant::TVariant(uint16 ui16) : private_(kMetaTypeUInt16) {
  private_.data.ui16 = ui16;
}

TVariant::TVariant(int32 i32) : private_(kMetaTypeInt32) {
  private_.data.i32 = i32;
}

TVariant::TVariant(uint32 ui32) : private_(kMetaTypeUInt32) {
  private_.data.ui32 = ui32;
}

TVariant::TVariant(int64 i64) : private_(kMetaTypeInt64) {
  private_.data.i64 = i64;
}

TVariant::TVariant(uint64 ui64) : private_(kMetaTypeUInt64) {
  private_.data.ui64 = ui64;
}

TVariant::TVariant(float f) : private_(kMetaTypeFloat) {
  private_.data.f = f;
}

TVariant::TVariant(double d) : private_(kMetaTypeDouble) {
  private_.data.d = d;
}

TVariant::TVariant(char c) : private_(kMetaTypeChar) {
  private_.data.c = c;
}

TVariant::TVariant(int32 type_id, const void* copy) {
  Create(type_id, copy);
}

TVariant::TVariant(const TVariant& other) {
  if (other.private_.is_shared) {
    private_ = other.private_;
    ++private_.data.shared->ref;
  } else {
    Create(other.private_.type, other.ConstData());
  }
}

int32 TVariant::TypeId() const {
  return private_.type;
}

const void* TVariant::ConstData() const {
  return private_.is_shared ? private_.data.shared->ptr : &private_.data.ptr;
}

void TVariant::Create(int32 type_id, const void* copy) {
  private_.type = type_id;
  TVariantConstruct(&private_, copy);
}

}  // namespace TMT
