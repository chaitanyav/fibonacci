require "bundler/setup"
require "rbconfig"

EXT_DIR = File.expand_path("../ext/fibonacci", __dir__)
EXT_NAME = "fibonacci_ext"
EXT_FILE = File.join(EXT_DIR, "#{EXT_NAME}.#{RbConfig::CONFIG['DLEXT']}")

unless File.exist?(EXT_FILE)
  Dir.chdir(EXT_DIR) do
    system(RbConfig.ruby, "extconf.rb") or raise "Failed to generate Makefile for the extension"
    system("make") or raise "Failed to build the extension"
  end
end

$LOAD_PATH.unshift(EXT_DIR) unless $LOAD_PATH.include?(EXT_DIR)
