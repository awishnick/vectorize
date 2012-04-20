#pragma once

#include <cmath>
#include <algorithm>
#include <xmmintrin.h>

namespace vectorize {

  struct Placeholder {
    template <class T>
    T apply(T x) const { return x; }
  };

  class Constant {
  public:
    Constant(float value) : _value(value) {}
    __m128 apply(__m128) const { return _mm_set1_ps(_value); }
    float apply(float) const { return _value; }
  private:
    float _value;
  };

  template <class L, class R, class Op>
  class BinOp {
  public:
    BinOp(const L& l, const R& r) : _l(l), _r(r) {}

    template <class T>
    T apply(T x) const {
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

    __m128 apply(__m128 x) const {
      return _t.apply(x);
    }
    float apply(float x) const {
      return _t.apply(x);
    }
  private:
    const T _t;
  };

  const Expr<Placeholder> _x;

  // Macro for generating binary operators. OpName is the C++ function
  // the user calls, i.e. operator+ or max. BinOpFn is the expression
  // template class that implements the operator.
#define GENERATE_BINARY_OPERATOR(OpName, BinOpFn)                     \
  template <class L, class R>                                         \
  Expr<BinOp<Expr<L>, Expr<R>, BinOpFn>>                              \
  OpName(const Expr<L>& l, const Expr<R>& r) {                        \
    typedef BinOp<Expr<L>, Expr<R>, BinOpFn> binop_type;              \
    return Expr<binop_type>(binop_type(l, r));                        \
  }                                                                   \
  template <class L>                                                  \
  Expr<BinOp<Expr<L>, Expr<Constant>, BinOpFn>>                       \
  OpName(const Expr<L>& l, float r) {                                 \
    typedef BinOp<Expr<L>, Expr<Constant>, BinOpFn> binop_type;       \
    Expr<Constant> c= Constant(r);                                    \
    return Expr<binop_type>(binop_type(l, c));                        \
  }                                                                   \
  template <class R>                                                  \
  Expr<BinOp<Expr<Constant>, Expr<R>, BinOpFn>>                       \
  OpName(float l, const Expr<R>& r) {                                 \
    typedef BinOp<Expr<Constant>, Expr<R>, BinOpFn> binop_type;       \
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

    __m128 apply(__m128 x) const {
      return Op::eval(_t.apply(x));
    }
    float apply(float x) const {
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

  // Macro for generating unary operators. OpName is the C++ function
  // the user calls, i.e. abs or sqrt. UnOpFn is the expression
  // template class that implements the operator.
#define GENERATE_UNARY_OPERATOR(OpName, UnOpFn)                     \
  template <class T>                                                \
  Expr<UnaryOp<Expr<T>, UnOpFn>> OpName(const Expr<T>& t) {         \
    typedef UnaryOp<Expr<T>, UnOpFn> unop_type;                     \
    return Expr<unop_type>(unop_type(t));                           \
  }

  GENERATE_UNARY_OPERATOR(abs, AbsOp)
  GENERATE_UNARY_OPERATOR(sqrt, SqrtOp)

#undef GENERATE_UNARY_OPERATOR

  template <class F>
  void apply(unsigned n, const float* src, float* target, F f) {
    for (; n>=4; n-=4, src+=4, target+=4) {
      _mm_store_ps(target, f.apply(_mm_load_ps(src)));
    }

    for (; n>0; --n, ++src, ++target) {
      *target= f.apply(*src);
    }
  }

}

