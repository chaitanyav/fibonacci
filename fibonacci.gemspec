# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "fibonacci/version"

Gem::Specification.new do |s|
  s.name        = "fibonacci"
  s.version     = VERSION
  s.authors     = ["Chaitanya Vellanki"]
  s.email       = ["me@chaitanyavellanki.com"]
  s.homepage    = "http://github.com/chaitanyav/fibonacci"
  s.summary     = %q{Fibonacci}
  s.description = %q{A Ruby gem for exploring Fibonacci series}

  s.rubyforge_project = "fibonacci"

  s.files         = Dir.glob('lib/**/*.rb') + Dir.glob('ext/**/*.{c,h,rb}')
  s.extensions = ['ext/fibonacci/extconf.rb']
  s.test_files    = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]

  # specify any dependencies here; for example:
  # s.add_development_dependency "rspec"
  # s.add_runtime_dependency "rest-client"
end
