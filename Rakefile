require "bundler/gem_tasks"
require "rake/testtask"
require "rbconfig"

EXT_DIR = File.expand_path("ext/fibonacci", __dir__)
EXT_NAME = "fibonacci_ext"
EXT_FILE = File.join(EXT_DIR, "#{EXT_NAME}.#{RbConfig::CONFIG['DLEXT']}")

file EXT_FILE => File.join(EXT_DIR, "extconf.rb") do
  Dir.chdir(EXT_DIR) do
    sh RbConfig.ruby, "extconf.rb"
    sh "make"
  end
end

task compile_extension: EXT_FILE

Rake::TestTask.new do |t|
  t.libs << "lib"
  t.libs << EXT_DIR
  t.test_files = FileList['test/**/*_test.rb']
  t.verbose = true
end

task test: :compile_extension

task default: :test
