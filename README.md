# TestBench MRuby <-> Ruby Compatibility

## Installation

MRuby compilation is controlled by `build_config.rb`. Copy `build_config.rb.example` to `build_config.rb`

    cp build_config.rb.example build_config.rb

The example file should suffice for getting started. For more information, see the [MRuby documentation](https://github.com/mruby/mruby/blob/master/doc/guides/compile.md)

Compile MRuby with `mruby-local` (it will download the [MRuby source code](https://github.com/mruby/mruby) the first time it is run):

    ./compile-mruby.sh

The compilation step produces executable files that get placed in `./bin`. Either add `bin` to `$PATH`, or else copy these files to a directory already in `$PATH`.

Test the `bench-mruby` executable by running it:

    > ./bin/bench-mruby
    Running test/automated/example.rb
    MRuby
      TestBench
        Constant is defined

    Finished running 1 file
    Ran 1 test in 0.001s (1000.0 tests/second)
    1 passed, 0 skipped, 0 failed, 0 total errors

The `mruby-require` executable can be used to run a single test file, similar to `ruby test/automated/some_file.rb`. Do not use the `mruby` executable itself in this manner, as it does not work with the `require` implementation provided by this project. Test `mruby-require`:

    > ./bin/mruby-require test/automated/example.rb
    Running test/automated/example.rb
    MRuby
      TestBench
        Constant is defined

## Testing Ruby Projects With bench-mruby

A typical TestBench project will install gem dependencies locally under e.g. `./gems`, which are loaded in development via `init.rb`. Projects that follow this structure can be tested with `bench-mruby` (as well as `mruby-require` for running single files). The following example assumes that the executables compiled previously are available in the shell's executable path.

    > cd /path/to/project

    # Install gem dependencies
    > ./install-gems.sh

    # Run the test suite normally with Ruby
    > ruby test/automated.rb

    # Run the test suite with MRuby
    > bench-mruby test/automated.rb

    # Run a single file with MRuby
    > mruby-require test/automated/some_test.rb

## Incompatibilities / Notes

- **Time.new** - The final two arguments to `Time.new` differ between MRI and mruby:
  - In MRI, the penultimate argument is a decimal value for seconds (e.g. `1.11` means one second and 11 milliseconds), and the final argument is the time zone offset.
  - In mruby, the penultimate argument is an integer value for seconds, and the final argument is an integer value for the number of microseconds. In the above example of `1.11`, you'd pass `1` and `110000` as the final two arguments to mruby.

- **Standard libraries** - The mruby equivalent of Ruby's standard libraries cannot be required; they must be compiled in to mruby itself. This means e.g. `require 'json'` must be "guarded" by a check for mruby, e.g. `require 'json' unless RUBY_ENGINE == 'mruby'`. This also applies to `require 'test_bench'` and `require 'test_bench/fixture'`, as they are compiled in to MRuby, too.

- **defined?** - `defined?` does not work on mruby, which means e.g. `defined?(Foo::Bar)` must be written as `Object.const_defined?(:Foo) && Foo.const_defined?(:Bar)`

- **Backtrace locations** - In MRI, both call stacks and backtraces (which are just call stacks stored in an exception) come in two variants, `caller`/`backtrace` (which return an array of strings) and `caller_locations`/`backtrace_locations` (which return an array of `Thread::Backtrace::Location` objects). MRuby only has `caller` and `backtrace`, not `caller_locations` and `backtrace_locations`

- **Struct instance variables** - Struct classes cannot have attributes like `attr_accessor`, since mruby does not allow instance variables on core classes.

- **Heredoc** - Heredoc support in mruby is limited to `<<TEXT`; `<<~TEXT` is not supported

- **Exit** Kernel's `#exit` in MRuby does not raise a SystemExit exception, `raise SystemExit.new(1)` must be used

- **Warn** Kernel's `#warn` is not available in MRuby without introducing a backfill

- **StringIO puts** StringIO's `#puts` does not print a newline character when no arguments are given (it does in MRI)

- **Tempfile directory** Tempfiles in MRuby must have a directory specified when a filename is given. Supplying a directory in this case doesn't cause a problem for MRI

- **Array * method** In MRI, `Array#*` is a multi purpose method: `[0] * 3` is `[0, 0, 0]`, and `['a', 'b', 'c'] * '+'` is `"a+b+c"`. The method isn't in MRuby, and backfilling support would be more trouble than it's worth, as all the different purposes of the method would have to be ported.

- **String methods on Symbol** Even with `mruby-symbol-ext` compiled in to MRuby, Symbol does not support all the string methods under MRuby that it does under MRI.

## License

The `mruby-ruby-compat` library is released under the [MIT License](https://github.com/test-bench/mruby-ruby-compat/blob/master/MIT-License.txt).
