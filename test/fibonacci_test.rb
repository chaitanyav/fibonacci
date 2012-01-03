require 'minitest/autorun'
require 'fibonacci'

class FibonacciTest < MiniTest::Unit::TestCase
  def setup
    @fib = Fibonacci.new
  end

  def test_brackets_method
    assert_equal 0, @fib[0]
    assert_equal 1, @fib[1]
    assert_equal 34, @fib[9]
    assert_equal 55, @fib[10]
    assert_equal 354224848179261915075, @fib[100]

    assert_raises ArgumentError do
      @fib[-1]
    end

    assert_raises ArgumentError do
      @fib["12"]
    end

    assert_raises ArgumentError do
      @fib[1.0]
    end
  end

  def test_terms
    assert_equal [0, 1, 1, 2, 3, 5], @fib.terms(6)
    assert_equal [], @fib.terms(0)
    assert_equal [0], @fib.terms(1)

    assert_raises ArgumentError do
      @fib.terms(-1)
    end

    assert_raises TypeError do
      @fib.terms("12")
    end

    assert_raises RangeError do
      @fib.terms(100000000000000000000000000000)
    end
  end

  def test_fast_val
    assert_equal 0, @fib.fast_val(0)
    assert_equal 1, @fib.fast_val(1)
    assert_equal 34, @fib.fast_val(9)
    assert_equal 55, @fib.fast_val(10)
    assert_equal 354224848179261915075, @fib.fast_val(100)

    assert_raises ArgumentError do
      @fib.fast_val(-1)
    end

    assert_raises ArgumentError do
      @fib.fast_val("12")
    end

    assert_raises ArgumentError do
      @fib.fast_val(1.0)
    end
  end

  def test_matrix
    assert_equal 0, (@fib.matrix(0))[0][1]
    assert_equal 1, (@fib.matrix(1))[0][1]
    assert_equal 34, (@fib.matrix(9))[0][1]
    assert_equal 55, (@fib.matrix(10))[0][1]
    assert_equal 354224848179261915075, (@fib.matrix(100))[0][1]

    matrix = @fib.matrix(100)
    assert_equal (@fib.matrix(101))[0][1], (matrix[0][1] + matrix[1][1])

    assert_raises ArgumentError do
      @fib.matrix(-1)
    end

    assert_raises ArgumentError do
      @fib.matrix("12")
    end

    assert_raises ArgumentError do
      @fib.matrix(1.0)
    end
  end
end
