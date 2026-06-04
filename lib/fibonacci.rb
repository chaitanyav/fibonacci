begin
  require 'fibonacci_ext'
rescue LoadError
  $LOAD_PATH.unshift File.expand_path('../ext/fibonacci', __dir__)
  require 'fibonacci_ext'
end
require "fibonacci/version"
