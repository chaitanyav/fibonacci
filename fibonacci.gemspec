# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "fibonacci/version"

Gem::Specification.new do |s|
  s.name        = "fibonacci"
  s.version     = VERSION
  s.authors     = ["NagaChaitanya Vellanki"]
  s.email       = ["nagachaitanya.vellanki.com"]
  s.homepage    = "http://github.com/chaitanyav/fibonacci"
  s.summary     = %q{Fibonacci}
  s.description = %q{A Ruby gem for exploring Fibonacci series}
  s.license     = "MIT"
  s.required_ruby_version = ">= 2.7"

  s.rubyforge_project = "fibonacci"

  s.files = Dir.glob('lib/**/*') + Dir.glob('ext/**/*.{c,h,rb}') + %w[README.markdown LICENSE Rakefile fibonacci.gemspec]
  s.extensions = ['ext/fibonacci/extconf.rb']
  s.test_files = `git ls-files -- {test,spec,features}/*`.split("\n")
  s.executables = `git ls-files -- bin/*`.split("\n").map { |f| File.basename(f) }
  s.require_paths = ["lib"]

  s.metadata = {
    "homepage_uri" => s.homepage,
    "source_code_uri" => "https://github.com/chaitanyav/fibonacci",
    "changelog_uri" => "https://github.com/chaitanyav/fibonacci/releases"
  }

  s.add_development_dependency "rake", ">= 13.0"
  s.add_development_dependency "minitest", ">= 5.0"
end
