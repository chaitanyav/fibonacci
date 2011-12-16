/******************************************************************************
 *
 * Fibonacci Series
 *
 * Author: NagaChaitanya Vellanki
 *
 * ***************************************************************************/

#include "ruby.h"

#define ONE INT2NUM(1)
#define ZERO INT2NUM(0)
#define TWO INT2NUM(2)
#define THREE INT2NUM(3)
#define MINUS_ONE INT2NUM(-1)
#define ARY_LEN 2L

static VALUE cFibonacci;
static ID id_plus, id_lte, id_gte, id_lt, id_pow, id_minus, id_fdiv, id_div;
static ID id_to_i, id_log10, id_floor, id_sqrt, id_mul, id_eq, id_not_eq;
static ID id_mod, id_bit_and, id_log2;

static VALUE
fibonacci_init(VALUE self)
{
    return self;
}

static VALUE
rb_print_num(VALUE num)
{
    VALUE num_str =  rb_funcall(num, rb_intern("to_s"), 0);
    char *cptr = StringValuePtr(num_str);
    printf("%s\n", cptr);
    return Qnil;
}


/*  call-seq:
 *    fib.fast_val(n)
 *
 *  Returns a Fixnum or Bignum.
 *
 *   fib.fast_val(100)
 *   #=> 354224848179261915075
 *
 *   fib.fast_val(10)
 *   #=> 55
 *
 *   fib.fast_val(200)
 *   #=> 280571172992510140037611932413038677189525
 *
 *
 *  ref: Daisuke Takahashi, A fast algorithm for computing large Fibonacci
 *  numbers, Information Processing Letters, Volume 75, Issue 6, 30 November
 *  2000, Pages 243-246, ISSN 0020-0190, 10.1016/S0020-0190(00)00112-5.
 */

static VALUE
rb_fast_val(VALUE self, VALUE n)
{
    VALUE f, l, sign, mask, log2, i, logn, logn_min_1, temp;

    if(TYPE(n) != T_FIXNUM)
    {
        rb_raise(rb_eArgError, "Invalid argument for type Fixnum");
        return Qnil;
    }

    if(RTEST(rb_funcall(n, id_lt, 1, ZERO)))
    {
        rb_raise(rb_eArgError, "n cannot be negative");
        return Qnil;
    }
    else
    {
      if(rb_equal(n, ZERO))
      {
        return ZERO;
      }
      else if(rb_equal(n, ONE))
      {
        return ONE;
      }
      else if(rb_equal(n, TWO))
      {
        return ONE;
      }
      else
      {
        f = ONE;
        l = ONE;
        sign = MINUS_ONE;
        logn = rb_funcall(rb_mMath, id_log2, 1, n);
        logn = rb_funcall(logn, id_floor, 0);
        logn_min_1 = rb_funcall(logn, id_minus, 1, ONE);
        mask = rb_funcall(TWO, id_pow, 1, logn_min_1);
        for(i = ONE; RTEST(rb_funcall(i, id_lte, 1, logn_min_1)); i = rb_funcall(i, id_plus, 1, ONE))
        {
          temp = rb_funcall(f, id_mul, 1, f);
          f = rb_funcall(f, id_plus, 1, l);
          f = rb_funcall(f, id_div, 1, TWO);
          f = rb_funcall(rb_funcall(f, id_mul, 1, f), id_mul, 1, TWO);
          f = rb_funcall(f, id_minus, 1, rb_funcall(temp, id_mul, 1, THREE));
          f = rb_funcall(f, id_minus, 1, rb_funcall(sign, id_mul, 1, TWO));
          l = rb_funcall(temp, id_mul, 1, INT2NUM(5));
          l = rb_funcall(l, id_plus, 1, rb_funcall(TWO, id_mul, 1, sign));
          sign = ONE;
          if(!rb_equal(rb_funcall(n, id_bit_and, 1, mask), ZERO))
          {
            temp = f;
            f = rb_funcall(f, id_plus, 1, l);
            f = rb_funcall(f, id_div, 1, TWO);
            l = rb_funcall(TWO, id_mul, 1, temp);
            l = rb_funcall(l, id_plus, 1, f);
            sign = MINUS_ONE;
          }
          mask = rb_funcall(mask, id_div, 1, TWO);
        }
          if(rb_equal(rb_funcall(n, id_bit_and, 1, mask), ZERO))
          {
            f = rb_funcall(f, id_mul, 1, l);
          }
          else
          {
            f = rb_funcall(f, id_plus, 1, l);
            f = rb_funcall(f, id_div, 1, TWO);
            f = rb_funcall(f, id_mul, 1, l);
            f = rb_funcall(f, id_minus, 1, sign);
          }
      }
    }

    return f;
}

static VALUE
rb_matrix_mul(VALUE ary1, VALUE ary2)
{
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

    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            for(k = 0; k < 2; k++)
            {
                /* tmp[i][j] = (tmp[i][j] + ary1[i][k] * ary2[k][j]); */
                temp = rb_funcall(rb_ary_entry(rb_ary_entry(ary1, i), k), id_mul,
                        1, rb_ary_entry(rb_ary_entry(ary2, k), j));
                rb_ary_store(rb_ary_entry(tmp_ary, i), j, rb_funcall(temp, id_plus, 1, rb_ary_entry(rb_ary_entry(tmp_ary, i), j)));
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
 *
 *   fib.matrix(100)
 *   #=> [[573147844013817084101, 354224848179261915075], [354224848179261915075,218922995834555169026]]
 *
 *   arr = fib.matrix(15)
 *   #=> [[987, 610], [610, 377]]
 *
 *   arr[0][1] or arr[1][0] is the value of nth term
 *
 *  Refer to http://en.wikipedia.org/wiki/Fibonacci_number#Matrix_form
 */

static VALUE
rb_matrix_form(VALUE self, VALUE n)
{
    VALUE base_ary;
    VALUE res_ary;
    VALUE tmp_ary;
    long ary_len = 2;

    if(TYPE(n) != T_FIXNUM)
    {
        rb_raise(rb_eArgError, "Invalid argument for type Fixnum");
        return Qnil;
    }

    if(RTEST(rb_funcall(n, id_lt, 1, ZERO)))
    {
        rb_raise(rb_eArgError, "n cannot be negative");
        return Qnil;
    }
    else
    {
        base_ary = rb_ary_new2(ARY_LEN);
        res_ary =  rb_ary_new2(ARY_LEN);
        tmp_ary = rb_ary_new2(ARY_LEN);

        /* base is {{1, 1}, {1, 0}} */
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

        while(!rb_equal(n, ZERO))
        {
            if(rb_equal(rb_funcall(n, id_mod, 1, TWO), ZERO))
            {
                n = rb_funcall(n, id_div, 1, TWO);
                base_ary = rb_matrix_mul(base_ary, base_ary);
            }
            else
            {
                n = rb_funcall(n, id_minus, 1, ONE);
                res_ary = rb_matrix_mul(res_ary, base_ary);
            }
        }
    }

    return res_ary;
}

/*  call-seq:
 *    fib[n]
 *
 *  Returns a Fixnum or Bignum.
 *
 *   fib[100]
 *   #=> 354224848179261915075
 *
 *   fib[10]
 *   #=> 55
 *
 *   fib[200]
 *   #=> 280571172992510140037611932413038677189525
 *
 *  The value of nth term is calculated iteratively.
 *
 *  Refer to http://en.wikipedia.org/wiki/Fibonacci_number#First_identity
 */

static VALUE
rb_iterative_val(VALUE self, VALUE n)
{
    VALUE start = ZERO;
    VALUE fib_n_1 = ONE;
    VALUE fib_n_2 = ZERO;
    VALUE fib_n = ZERO;

    if(TYPE(n) != T_FIXNUM)
    {
        rb_raise(rb_eArgError, "Invalid argument for type Fixnum");
        return Qnil;
    }

    if(RTEST(rb_funcall(n, id_lt, 1, ZERO)))
    {
        rb_raise(rb_eArgError, "n cannot be negative");
        return Qnil;
    }
    else
    {

        for(start; RTEST(rb_funcall(start, id_lte, 1, n)); start = rb_funcall(start, id_plus, 1, ONE))
        {
            if(rb_equal(start, ZERO))
            {
                fib_n = ZERO;
            }
            else if(rb_equal(start, ONE))
            {
                fib_n = ONE;
            }
            else
            {
                fib_n = rb_funcall(fib_n_1, id_plus, 1, fib_n_2);
                fib_n_2 = fib_n_1;
                fib_n_1 = fib_n;
            }
        }
    }
    return fib_n;
}

/*  call-seq:
 *    fib.terms(n)
 *
 *  Returns a array with the first n terms of the series
 *
 *   fib.terms(5)
 *   #=> [0, 1, 1, 2, 3]
 *
 *   fib.terms(10)
 *   #=> [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
 *
 *   fib.terms(15)
 *   #=> [0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377]
 *
 *   fib.terms(0)
 *   #=> []
 *
 *  Refer to http://en.wikipedia.org/wiki/Fibonacci_number#First_identity
 */

static VALUE
terms(VALUE self, VALUE n)
{
    long ary_len = NUM2LONG(n);
    long i;
    VALUE ary = Qnil;

    if(ary_len < 0)
    {
        rb_raise(rb_eArgError, "num terms cannot be negative");
        return ary;
    }

    ary = rb_ary_new2(ary_len);

    for(i=0; i < ary_len; i++)
    {
        if(i == 0)
        {
            rb_ary_store(ary, i, ZERO);
        }
        if((i > 0))
        {
            if(i <= 2)
            {
                rb_ary_store(ary, i, ONE);
            }
            else
            {
                rb_ary_store(ary, i, rb_funcall(rb_ary_entry(ary, i-1), id_plus, 1, rb_ary_entry(ary, i-2)));
            }
        }
    }
    return ary;
}

/*  call-seq:
 *    fib.print(n)
 *
 *  Prints the first n terms of the series.
 *
 *   fib.print(1)
 *   #=>   0
 *
 *   fib.print(2)
 *   #=>   0
 *         1
 *   fib.print(5)
 *   #=>    0
 *          1
 *          1
 *          2
 *          3
 *
 */

static VALUE
print(VALUE self, VALUE n)
{
    VALUE start = ZERO;
    VALUE fib_n_1 = ONE;
    VALUE fib_n_2 = ZERO;
    VALUE fib_n = ZERO;

    if(TYPE(n) != T_FIXNUM)
    {
        rb_raise(rb_eArgError, "Invalid argument for type Fixnum");
        return Qnil;
    }

    for(start; RTEST(rb_funcall(start, id_lt, 1, n)); start = rb_funcall(start, id_plus, 1, ONE))
    {
        if(rb_equal(start, ZERO))
        {
            rb_print_num(ZERO);
        }
        else if(rb_equal(start, ONE))
        {
            rb_print_num(ONE);
        }
        else
        {
            fib_n = rb_funcall(fib_n_1, id_plus, 1, fib_n_2);
            fib_n_2 = fib_n_1;
            fib_n_1 = fib_n;
            rb_print_num(fib_n);
        }
    }

    return Qnil;
}

static VALUE
index_of(VALUE self, VALUE val)
{
    return Qnil;
}

/*  call-seq:
 *    fib.num_digits(n)
 *
 *  Returns the number of digits in the nth term of the series
 *
 *   fib.num_digits(10)
 *   #=> 2
 *
 *   fib.num_digits(100)
 *   #=> 21
 *
 *  Refer to http://en.wikipedia.org/wiki/Fibonacci_number#Computation_by_rounding
 */

static VALUE
num_digits(VALUE self, VALUE n)
{
    if(TYPE(n) != T_FIXNUM)
    {
        rb_raise(rb_eArgError, "Invalid argument for type Fixnum");
        return Qnil;
    }

    if(RTEST(rb_funcall(n, id_lt, 1, ZERO)))
    {
        rb_raise(rb_eArgError, "n cannot be negative");
        return Qnil;
    }
    else
    {
        VALUE phi = ONE;
        VALUE num_digits = ZERO;
        VALUE log_sqrt_5 = ZERO;
        VALUE sqrt_5;

        if(rb_equal(n, ZERO))
        {
            return ZERO;
        }

        /*  work around since the value log(phi/sqrt(5)) + 1 = 0.8595026380819693
         *  converting to integer would be zero */
        if(rb_equal(n, ONE))
        {
            return ONE;
        }

        if(RTEST(rb_funcall(n, id_gte, 1, TWO)))
        {
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
    }
}

void
Init_fibonacci(void)
{
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
    id_mod = rb_intern("!=");
    id_bit_and = rb_intern("&");

    cFibonacci = rb_define_class("Fibonacci", rb_cObject);
    rb_define_method(cFibonacci, "initialize", fibonacci_init, 0);
    rb_define_method(cFibonacci, "print", print, 1);
    rb_define_method(cFibonacci, "terms", terms, 1);
    rb_define_method(cFibonacci, "num_digits", num_digits, 1);
    rb_define_method(cFibonacci, "[]", rb_iterative_val, 1);
    rb_define_method(cFibonacci, "matrix", rb_matrix_form, 1);
    rb_define_method(cFibonacci, "fast_val", rb_fast_val, 1);
}
