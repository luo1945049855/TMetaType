#include "TMetaType.h"
#include "TVariant.h"

#include "gtest/gtest.h"

class A {
 public:
  TMETA_CLASS(A)

  A() : value_a_(.0) { std::cout << "A Default Constructor" << std::endl; }
  A(double a) : value_a_(a) { std::cout << "A Constructor" << std::endl; }
  virtual ~A() { std::cout << "A Destructor" << std::endl; }

  double GetValueA() const { return value_a_; }

  double value_a_;
};

class B : public A {
 public:
  TMETA_CLASS(B)

  B() { std::cout << "B Default Constructor" << std::endl; }
  B(double a, double b) : A(a), value_b_(b) {
    std::cout << "B Constructor" << std::endl;
  }
  virtual ~B() { std::cout << "B Destructor" << std::endl; }

  void SetValueB(double value) { value_b_ = value; }
  double GetValueB() const { return value_b_; }

  double value_b_;
};

BEGIN_META_CLASS(A)
ADD_META_PROPERTY_READONLY(A, double, ValueA)
END_META_CLASS()

BEGIN_META_CLASS(B)
TMETA_CLASS_PARENT(A)
ADD_META_PROPERTY(B, double, ValueB)
END_META_CLASS()

TEST(MetaType, TMetaObject) {
  EXPECT_EQ(TMT::TMetaTypeId<bool>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("bool"));
  EXPECT_EQ(TMT::TMetaTypeId<int8>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("int8"));
  EXPECT_EQ(TMT::TMetaTypeId<uint8>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("uint8"));
  EXPECT_EQ(TMT::TMetaTypeId<int16>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("int16"));
  EXPECT_EQ(TMT::TMetaTypeId<uint16>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("uint16"));
  EXPECT_EQ(TMT::TMetaTypeId<int32>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("int32"));
  EXPECT_EQ(TMT::TMetaTypeId<uint32>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("uint32"));
  EXPECT_EQ(TMT::TMetaTypeId<int64>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("int64"));
  EXPECT_EQ(TMT::TMetaTypeId<uint64>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("uint64"));
  EXPECT_EQ(TMT::TMetaTypeId<float>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("float"));
  EXPECT_EQ(TMT::TMetaTypeId<double>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("double"));
  EXPECT_EQ(TMT::TMetaTypeId<char>::MetaTypeId(),
            TMT::TMetaType::GetTypeId("char"));

  const int32 type_id_a = TMT::TMetaType::GetTypeId("A");
  const int32 type_id_b = TMT::TMetaType::GetTypeId("B");

  const TMT::TMetaObject* meta_obj_a = TMT::TMetaType::GetMetaObject(type_id_a);
  const TMT::TMetaObject* meta_obj_b = TMT::TMetaType::GetMetaObject(type_id_b);

  EXPECT_EQ(type_id_a, A::MetaTypeId());
  EXPECT_EQ(type_id_b, B::MetaTypeId());
  EXPECT_EQ(meta_obj_a, A::MetaObject());
  EXPECT_EQ(meta_obj_b, B::MetaObject());
  EXPECT_EQ(meta_obj_a, meta_obj_b->GetParentMetaObject());
}

TEST(MetaType, TVariant) {
  TMT::TVariant tv_i8(0x7f);
  TMT::TVariant tv_ui8(0xff);
  TMT::TVariant tv_i16(0x7fff);
  TMT::TVariant tv_ui16(0xffff);
  TMT::TVariant tv_i32(0x7fffffff);
  TMT::TVariant tv_ui32(0xffffffff);
  TMT::TVariant tv_i64(0x7fffffffffffffff);
  TMT::TVariant tv_ui64(0xffffffffffffffff);
  TMT::TVariant tv_true(true);
  TMT::TVariant tv_false(false);
  TMT::TVariant tv_float(0.01f);
  TMT::TVariant tv_double(0.01);
  TMT::TVariant tv_char('A');
  TMT::TVariant tv_char_lower('a');

  {
    A a{1.0};
    B b{1.1, 2.0};
    TMT::TVariant tv_a(A::MetaTypeId(), &a);
    TMT::TVariant tv_b(B::MetaTypeId(), &b);
    TMT::TVariant tv_copy_shared(tv_b);
    EXPECT_EQ(1.0, tv_a.Value<A>().value_a_);
    EXPECT_EQ(1.1, tv_b.Value<B>().value_a_);
    EXPECT_EQ(2.0, tv_b.Value<B>().value_b_);
    EXPECT_EQ(tv_b.ConstData(), tv_copy_shared.ConstData());
  }

  EXPECT_EQ(0x7f, tv_i8.Value<int8>());
  EXPECT_EQ(0xff, tv_ui8.Value<uint8>());
  EXPECT_EQ(0x7fff, tv_i16.Value<int16>());
  EXPECT_EQ(0xffff, tv_ui16.Value<uint16>());
  EXPECT_EQ(0x7fffffff, tv_i32.Value<int32>());
  EXPECT_EQ(0xffffffff, tv_ui32.Value<uint32>());
  EXPECT_EQ(0x7fffffffffffffff, tv_i64.Value<int64>());
  EXPECT_EQ(0xffffffffffffffff, tv_ui64.Value<uint64>());
  EXPECT_EQ(true, tv_true.Value<bool>());
  EXPECT_EQ(false, tv_false.Value<bool>());
  EXPECT_EQ(0.01f, tv_float.Value<float>());
  EXPECT_EQ(0.01, tv_double.Value<double>());
  EXPECT_EQ(65, tv_char.Value<char>());
  EXPECT_EQ(97, tv_char_lower.Value<char>());
}

TEST(MetaType, TMetaProperty) {
  A a{1.0};
  B b{1.1, 2.0};

  const TMT::TMetaObject* meta_obj_a = A::MetaObject();
  const TMT::TMetaObject* meta_obj_b = B::MetaObject();

  const TMT::TMetaProperty* meta_prop_value_a =
      meta_obj_a->GetProperty("ValueA");
  const TMT::TMetaProperty* meta_prop_value_b =
      meta_obj_b->GetProperty("ValueB");

  EXPECT_EQ(meta_prop_value_a->GetPropertyName(), "ValueA");
  EXPECT_EQ(meta_prop_value_a->GetPropertyType(), "double");
  EXPECT_EQ(meta_prop_value_a->GetPropValue(&a).Value<double>(), 1.0);
  EXPECT_EQ(meta_prop_value_a->GetPropValue(&b).Value<double>(), 1.1);
  EXPECT_EQ(meta_prop_value_b->GetPropertyName(), "ValueB");
  EXPECT_EQ(meta_prop_value_b->GetPropertyType(), "double");
  EXPECT_EQ(meta_prop_value_b->GetPropValue(&b).Value<double>(), 2.0);
}

