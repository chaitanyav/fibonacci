/******************************************************************************
 *
 * Fibonacci Series (Modern Ruby C API version)
 *
 * Author: NagaChaitanya Vellanki
 * Updated: 2026 - Ruby 3.0+ compatible
 *
 ***************************************************************************/

#include "ruby.h"

/* Ruby version compatibility */
#define RUBY_VERSION_MAJOR RUBY_API_VERSION_MAJOR
#define RUBY_VERSION_MINOR RUBY_API_VERSION_MINOR

#define ONE INT2NUM(1)
#define ZERO INT2NUM(0)
#define TWO INT2NUM(2)
#define THREE INT2NUM(3)
#define MINUS_ONE INT2NUM(-1)
#define ARY_LEN 2L

static VALUE cFibonacci;

/* Pre-cached method IDs - initialized at module load time */
static ID id_plus, id_lte, id_gte, id_lt, id_pow, id_minus, id_fdiv, id_div;
static ID id_to_i, id_log10, id_floor, id_sqrt, id_mul, id_eq, id_not_eq;
static ID id_mod, id_bit_and, id_log2, id_to_s;

/* Inline helper for safer integer type checking */
static inline int fib_is_valid_index(VALUE n) { return FIXNUM_P(n); }

/* Inline helper to check if number is negative */
static inline int fib_is_negative(VALUE n) {
  return RTEST(rb_funcall(n, id_lt, 1, ZERO));
}

/* Inline helper to compare equality with zero */
static inline int fib_is_zero(VALUE n) {
  return RTEST(rb_funcall(n, id_eq, 1, ZERO));
}

/* Inline helper for safe output */
static inline void fib_print_value(VALUE num) {
  VALUE num_str = rb_funcall(num, id_to_s, 0);
  printf("%s\n", StringValueCStr(num_str));
}

/*  call-seq:
 *    fib.fast_val(n)
 *
 *  Returns a Fixnum or Bignum using fast matrix-based algorithm.
 *
 *   fib.fast_val(100)
 *   #=> 354224848179261915075
 *
 *  ref: Daisuke Takahashi, A fast algorithm for computing large Fibonacci
 *  numbers, Information Processing Letters, Volume 75, Issue 6, 30 November
 *  2000, Pages 243-246, ISSN 0020-0190, 10.1016/S0020-0190(00)00112-5.
 */
static VALUE rb_fast_val(VALUE self, VALUE n) {
  VALUE f, l, sign, mask, i, logn, logn_min_1, temp;

  if (!fib_is_valid_index(n)) {
    rb_raise(rb_eArgError, "Fibonacci index must be an integer");
    return Qnil;
  }

  if (fib_is_negative(n)) {
    rb_raise(rb_eArgError, "Fibonacci index cannot be negative");
    return Qnil;
  }

  if (fib_is_zero(n)) {
    return ZERO;
  }
  if (rb_equal(n, ONE)) {
    return ONE;
  }
  if (rb_equal(n, TWO)) {
    return ONE;
  }

  f = ONE;
  l = ONE;
  sign = MINUS_ONE;
  logn = rb_funcall(rb_mMath, id_log2, 1, n);
  logn = rb_funcall(logn, id_floor, 0);
  logn_min_1 = rb_funcall(logn, id_minus, 1, ONE);
  mask = rb_funcall(TWO, id_pow, 1, logn_min_1);

  for (i = ONE; RTEST(rb_funcall(i, id_lte, 1, logn_min_1));
       i = rb_funcall(i, id_plus, 1, ONE)) {
    temp = rb_funcall(f, id_mul, 1, f);
    f = rb_funcall(f, id_plus, 1, l);
    f = rb_funcall(f, id_div, 1, TWO);
    f = rb_funcall(rb_funcall(f, id_mul, 1, f), id_mul, 1, TWO);
    f = rb_funcall(f, id_minus, 1, rb_funcall(temp, id_mul, 1, THREE));
    f = rb_funcall(f, id_minus, 1, rb_funcall(sign, id_mul, 1, TWO));
    l = rb_funcall(temp, id_mul, 1, INT2NUM(5));
    l = rb_funcall(l, id_plus, 1, rb_funcall(TWO, id_mul, 1, sign));
    sign = ONE;

    if (!rb_equal(rb_funcall(n, id_bit_and, 1, mask), ZERO)) {
      temp = f;
      f = rb_funcall(f, id_plus, 1, l);
      f = rb_funcall(f, id_div, 1, TWO);
      l = rb_funcall(TWO, id_mul, 1, temp);
      l = rb_funcall(l, id_plus, 1, f);
      sign = MINUS_ONE;
    }
    mask = rb_funcall(mask, id_div, 1, TWO);
  }

  if (rb_equal(rb_funcall(n, id_bit_and, 1, mask), ZERO)) {
    f = rb_funcall(f, id_mul, 1, l);
  } else {
    f = rb_funcall(f, id_plus, 1, l);
    f = rb_funcall(f, id_div, 1, TWO);
    f = rb_funcall(f, id_mul, 1, l);
    f = rb_funcall(f, id_minus, 1, sign);
  }

  return f;
}

static VALUE rb_matrix_mul(VALUE ary1, VALUE ary2) {
  long i, j, k;
  VALUE temp;
  VALUE tmp_ary = rb_ary_new2(ARY_LEN);
  VALUE zero_ary = rb_ary_new2(ARY_LEN);

  rb_ary_push(zero_ary, ZERO);
  rb_ary_push(zero_ary, ZERO);
  rb_ary_push(tmp_ary, zero_ary);

  zero_ary = rb_ary_new2(ARY_LEN);
  rb_ary_push(zero_ary, ZERO);
  rb_ary_push(zero_ary, ZERO);
  rb_ary_push(tmp_ary, zero_ary);

  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      for (k = 0; k < 2; k++) {
        temp = rb_funcall(rb_ary_entry(rb_ary_entry(ary1, i), k), id_mul, 1,
                          rb_ary_entry(rb_ary_entry(ary2, k), j));
        rb_ary_store(rb_ary_entry(tmp_ary, i), j,
                     rb_funcall(temp, id_plus, 1,
                                rb_ary_entry(rb_ary_entry(tmp_ary, i), j)));
      }
    }
  }

  return tmp_ary;
}

/*  call-seq:
 *    fib.matrix(n)
 *
 *  Returns a 2x2 matrix(2-dimensional array).
 *
 *   fib.matrix(10)
 *   #=> [[89, 55], [55, 34]]
 */
static VALUE rb_matrix_form(VALUE self, VALUE n) {
  VALUE base_ary, res_ary, tmp_ary;

  if (!fib_is_valid_index(n)) {
    rb_raise(rb_eArgError, "Matrix index must be an integer");
    return Qnil;
  }

  if (fib_is_negative(n)) {
    rb_raise(rb_eArgError, "Matrix index cannot be negative");
    return Qnil;
  }

  base_ary = rb_ary_new2(ARY_LEN);
  res_ary = rb_ary_new2(ARY_LEN);

  /* base is {{1, 1}, {1, 0}} */
  tmp_ary = rb_ary_new2(ARY_LEN);
  rb_ary_push(tmp_ary, ONE);
  rb_ary_push(tmp_ary, ONE);
  rb_ary_push(base_ary, tmp_ary);

  tmp_ary = rb_ary_new2(ARY_LEN);
  rb_ary_push(tmp_ary, ONE);
  rb_ary_push(tmp_ary, ZERO);
  rb_ary_push(base_ary, tmp_ary);

  /* res is {{1, 0}, {0, 1}} */
  tmp_ary = rb_ary_new2(ARY_LEN);
  rb_ary_push(tmp_ary, ONE);
  rb_ary_push(tmp_ary, ZERO);
  rb_ary_push(res_ary, tmp_ary);

  tmp_ary = rb_ary_new2(ARY_LEN);
  rb_ary_push(tmp_ary, ZERO);
  rb_ary_push(tmp_ary, ONE);
  rb_ary_push(res_ary, tmp_ary);

  while (!rb_equal(n, ZERO)) {
    if (rb_equal(rb_funcall(n, id_mod, 1, TWO), ZERO)) {
      n = rb_funcall(n, id_div, 1, TWO);
      base_ary = rb_matrix_mul(base_ary, base_ary);
    } else {
      n = rb_funcall(n, id_minus, 1, ONE);
      res_ary = rb_matrix_mul(res_ary, base_ary);
    }
  }

  return res_ary;
}

/*  call-seq:
 *    fib[n]
 *
 *  Returns the nth Fibonacci number (iterative calculation).
 *
 *   fib[100]
 *   #=> 354224848179261915075
 */
static VALUE rb_iterative_val(VALUE self, VALUE n) {
  VALUE start = TWO;
  VALUE fib_n_1 = ONE;
  VALUE fib_n_2 = ZERO;
  VALUE fib_n = ZERO;

  if (!fib_is_valid_index(n)) {
    rb_raise(rb_eArgError, "Index must be an integer");
    return Qnil;
  }

  if (fib_is_negative(n)) {
    rb_raise(rb_eArgError, "Index cannot be negative");
    return Qnil;
  }

  if (fib_is_zero(n)) {
    fib_n = ZERO;
  } else if (rb_equal(n, ONE)) {
    fib_n = ONE;
  } else {
    for (start; RTEST(rb_funcall(start, id_lte, 1, n));
         start = rb_funcall(start, id_plus, 1, ONE)) {
      fib_n = rb_funcall(fib_n_1, id_plus, 1, fib_n_2);
      fib_n_2 = fib_n_1;
      fib_n_1 = fib_n;
    }
  }

  return fib_n;
}

/*  call-seq:
 *    fib.terms(n)
 *
 *  Returns array with the first n terms of the series
 *
 *   fib.terms(5)
 *   #=> [0, 1, 1, 2, 3]
 */
static VALUE terms(VALUE self, VALUE n) {
  long ary_len = NUM2LONG(n);
  long i;
  VALUE ary;

  if (ary_len < 0) {
    rb_raise(rb_eArgError, "Number of terms cannot be negative");
    return Qnil;
  }

  ary = rb_ary_new2(ary_len);

  for (i = 0; i < ary_len; i++) {
    if (i == 0) {
      rb_ary_store(ary, i, ZERO);
    } else if (i <= 2) {
      rb_ary_store(ary, i, ONE);
    } else {
      rb_ary_store(ary, i,
                   rb_funcall(rb_ary_entry(ary, i - 1), id_plus, 1,
                              rb_ary_entry(ary, i - 2)));
    }
  }

  return ary;
}

/*  call-seq:
 *    fib.print(n)
 *
 *  Prints the first n terms of the series.
 *
 *   fib.print(5)
 *   #=>    0
 *          1
 *          1
 *          2
 *          3
 */
static VALUE print_terms(VALUE self, VALUE n) {
  VALUE start = ZERO;
  VALUE fib_n_1 = ONE;
  VALUE fib_n_2 = ZERO;
  VALUE fib_n = ZERO;

  if (!fib_is_valid_index(n)) {
    rb_raise(rb_eArgError, "Argument must be an integer");
    return Qnil;
  }

  if (fib_is_negative(n)) {
    rb_raise(rb_eArgError, "Argument cannot be negative");
    return Qnil;
  }

  for (start; RTEST(rb_funcall(start, id_lt, 1, n));
       start = rb_funcall(start, id_plus, 1, ONE)) {
    if (rb_equal(start, ZERO)) {
      fib_print_value(ZERO);
    } else if (rb_equal(start, ONE)) {
      fib_print_value(ONE);
    } else {
      fib_n = rb_funcall(fib_n_1, id_plus, 1, fib_n_2);
      fib_n_2 = fib_n_1;
      fib_n_1 = fib_n;
      fib_print_value(fib_n);
    }
  }

  return Qnil;
}

/*  call-seq:
 *    fib.num_digits(n)
 *
 *  Returns the number of digits in the nth Fibonacci number
 *
 *   fib.num_digits(100)
 *   #=> 21
 */
static VALUE num_digits(VALUE self, VALUE n) {
  VALUE phi, num_digits, log_sqrt_5, sqrt_5;

  if (!fib_is_valid_index(n)) {
    rb_raise(rb_eArgError, "Argument must be an integer");
    return Qnil;
  }

  if (fib_is_negative(n)) {
    rb_raise(rb_eArgError, "Argument cannot be negative");
    return Qnil;
  }

  if (fib_is_zero(n)) {
    return ZERO;
  }

  if (rb_equal(n, ONE)) {
    return ONE;
  }

  if (!RTEST(rb_funcall(n, id_gte, 1, TWO))) {
    return Qnil;
  }

  phi = ONE;
  num_digits = ZERO;
  log_sqrt_5 = ZERO;

  sqrt_5 = rb_funcall(rb_mMath, id_sqrt, 1, INT2NUM(5));
  log_sqrt_5 = rb_funcall(rb_mMath, id_log10, 1, sqrt_5);

  phi = rb_funcall(phi, id_plus, 1, sqrt_5);
  phi = rb_funcall(phi, id_fdiv, 1, TWO);

  num_digits = rb_funcall(rb_mMath, id_log10, 1, phi);
  num_digits = rb_funcall(num_digits, id_mul, 1, n);
  num_digits = rb_funcall(num_digits, id_minus, 1, log_sqrt_5);
  num_digits = rb_funcall(num_digits, id_floor, 0);
  num_digits = rb_funcall(num_digits, id_plus, 1, ONE);
  num_digits = rb_funcall(num_digits, id_to_i, 0);

  return num_digits;
}

static VALUE fibonacci_init(VALUE self) { return self; }

/* Initialize Ruby module and methods */
void Init_fibonacci_ext(void) {
  /* Cache commonly used method IDs at initialization time */
  id_plus = rb_intern("+");
  id_lte = rb_intern("<=");
  id_lt = rb_intern("<");
  id_gte = rb_intern(">=");
  id_pow = rb_intern("**");
  id_mul = rb_intern("*");
  id_minus = rb_intern("-");
  id_fdiv = rb_intern("fdiv");
  id_div = rb_intern("/");
  id_to_i = rb_intern("to_i");
  id_log10 = rb_intern("log10");
  id_log2 = rb_intern("log2");
  id_floor = rb_intern("floor");
  id_sqrt = rb_intern("sqrt");
  id_eq = rb_intern("==");
  id_not_eq = rb_intern("!=");
  id_mod = rb_intern("%");
  id_bit_and = rb_intern("&");
  id_to_s = rb_intern("to_s");

  /* Define the Fibonacci class */
  cFibonacci = rb_define_class("Fibonacci", rb_cObject);

  /* Define instance methods */
  rb_define_method(cFibonacci, "initialize", fibonacci_init, 0);
  rb_define_method(cFibonacci, "print", print_terms, 1);
  rb_define_method(cFibonacci, "terms", terms, 1);
  rb_define_method(cFibonacci, "num_digits", num_digits, 1);
  rb_define_method(cFibonacci, "[]", rb_iterative_val, 1);
  rb_define_method(cFibonacci, "matrix", rb_matrix_form, 1);
  rb_define_method(cFibonacci, "fast_val", rb_fast_val, 1);
}
