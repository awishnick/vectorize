#pragma once

#include <xmmintrin.h>
#include <cmath>
#include <algorithm>

namespace vectorize {

  template <unsigned N>
  struct Placeholder {
    template <class T, unsigned M>
    T apply(T (&x)[M]) const {
      static_assert(N < M,
                    "Too few arguments to placeholder expression,"
                    "this is probably due to passing an expression"
                    "with more placeholders than actual inputs.");
      return x[N];
    }
  };

  class Constant {
  public:
    Constant(float value) : _value(value) {}
    template <unsigned N>
    __m128 apply(__m128(&)[N]) const { return _mm_set1_ps(_value); }
    template <unsigned N>
    float apply(float(&)[N]) const { return _value; }
  private:
    float _value;
  };

  template <class L, class R, class Op>
  class BinOp {
  public:
    BinOp(const L& l, const R& r) : _l(l), _r(r) {}

    template <class T, unsigned N>
    T apply(T (&x)[N]) const {
      return Op::eval(_l.apply(x), _r.apply(x));
    }
  private:
    const L _l;
    const R _r;
  };

  struct AddOp {
    static __m128 eval(__m128 x, __m128 y) {
      return _mm_add_ps(x, y);
    }
    static float eval(float x, float y) {
      return x + y;
    }
  };

  struct SubOp {
    static __m128 eval(__m128 x, __m128 y) {
      return _mm_sub_ps(x, y);
    }
    static float eval(float x, float y) {
      return x - y;
    }
  };

  struct MulOp {
    static __m128 eval(__m128 x, __m128 y) {
      return _mm_mul_ps(x, y);
    }
    static float eval(float x, float y) {
      return x * y;
    }
  };

  struct MaxOp {
    static __m128 eval(__m128 x, __m128 y) {
      return _mm_max_ps(x, y);
    }
    static float eval(float x, float y) {
      return std::max(x, y);
    }
  };

  struct MinOp {
    static __m128 eval(__m128 x, __m128 y) {
      return _mm_min_ps(x, y);
    }
    static float eval(float x, float y) {
      return std::min(x, y);
    }
  };

  template <class T>
  class Expr {
  public:
    Expr(const T& t) : _t(t) {}
    Expr() : _t() {}

    template <unsigned N>
    __m128 apply(__m128 (&x)[N]) const {
      return _t.apply(x);
    }
    template <unsigned N>
    float apply(float (&x)[N]) const {
      return _t.apply(x);
    }
  private:
    const T _t;
  };

  const Expr<Placeholder<0>> _x;
  const Expr<Placeholder<1>> _y;

  // Macro for generating binary operators. OpFunction is the C++ function
  // the user calls, i.e. operator+ or max. BinOpClass is the expression
  // template class that implements the operator.
#define GENERATE_BINARY_OPERATOR(OpFunction, BinOpClass)              \
  template <class L, class R>                                         \
  Expr<BinOp<Expr<L>, Expr<R>, BinOpClass>>                           \
  OpFunction(const Expr<L>& l, const Expr<R>& r) {                    \
    typedef BinOp<Expr<L>, Expr<R>, BinOpClass> binop_type;           \
    return Expr<binop_type>(binop_type(l, r));                        \
  }                                                                   \
  template <class L>                                                  \
  Expr<BinOp<Expr<L>, Expr<Constant>, BinOpClass>>                    \
  OpFunction(const Expr<L>& l, float r) {                             \
    typedef BinOp<Expr<L>, Expr<Constant>, BinOpClass> binop_type;    \
    Expr<Constant> c= Constant(r);                                    \
    return Expr<binop_type>(binop_type(l, c));                        \
  }                                                                   \
  template <class R>                                                  \
  Expr<BinOp<Expr<Constant>, Expr<R>, BinOpClass>>                    \
  OpFunction(float l, const Expr<R>& r) {                             \
    typedef BinOp<Expr<Constant>, Expr<R>, BinOpClass> binop_type;    \
    Expr<Constant> c= Constant(l);                                    \
    return Expr<binop_type>(binop_type(c, r));                        \
  }                                                                   \

  GENERATE_BINARY_OPERATOR(operator+, AddOp)
  GENERATE_BINARY_OPERATOR(operator-, SubOp)
  GENERATE_BINARY_OPERATOR(operator*, MulOp)
  GENERATE_BINARY_OPERATOR(max, MaxOp)
  GENERATE_BINARY_OPERATOR(min, MinOp)

#undef GENERATE_BINARY_OPERATOR

  template <class T, class Op>
  class UnaryOp {
  public:
    UnaryOp(const T& t) : _t(t) {}

    template <unsigned N>
    __m128 apply(__m128 (&x)[N]) const {
      return Op::eval(_t.apply(x));
    }
    template <unsigned N>
    float apply(float (&x)[N]) const {
      return Op::eval(_t.apply(x));
    }
  private:
    const T _t;
  };

  struct AbsOp {
    static __m128 eval(__m128 x) {
      return _mm_andnot_ps(_mm_set1_ps(-0.0f), x);
    }
    static float eval(float x) {
      return std::abs(x);
    }
  };

  struct SqrtOp {
    static __m128 eval(__m128 x) {
      return _mm_sqrt_ps(x);
    }
    static float eval(float x) {
      return std::sqrt(x);
    }
  };

  // Macro for generating unary operators. OpFunction is the C++ function
  // the user calls, i.e. abs or sqrt. UnOpClass is the expression
  // template class that implements the operator.
#define GENERATE_UNARY_OPERATOR(OpFunction, UnOpClass)              \
  template <class T>                                                \
  Expr<UnaryOp<Expr<T>, UnOpClass>> OpFunction(const Expr<T>& t) {  \
    typedef UnaryOp<Expr<T>, UnOpClass> unop_type;                  \
    return Expr<unop_type>(unop_type(t));                           \
  }

  GENERATE_UNARY_OPERATOR(abs, AbsOp)
  GENERATE_UNARY_OPERATOR(sqrt, SqrtOp)

#undef GENERATE_UNARY_OPERATOR

  template <class F>
  void apply(unsigned n, const float* src, float* target, F f) {
    for (; n>=4; n-=4, src+=4, target+=4) {
      __m128 x[]= { _mm_load_ps(src) };
      _mm_store_ps(target, f.apply(x));
    }

    for (; n>0; --n, ++src, ++target) {
      float x[]= { *src };
      *target= f.apply(x);
    }
  }

  template <class F>
  void apply2(unsigned n, const float* src1, const float* src2,
              float* target, F f) {
    for (; n>=4; n-=4, src1+=4, src2+=4, target+=4) {
      __m128 x[]= { _mm_load_ps(src1), _mm_load_ps(src2) };
      _mm_store_ps(target, f.apply(x));
    }

    for (; n>0; --n, ++src1, ++src2, ++target) {
      float x[]= { *src1, *src2 };
      *target= f.apply(x);
    }
  }

}

