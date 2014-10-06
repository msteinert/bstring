[![Build Status](https://travis-ci.org/msteinert/bstring.png?branch=master)](https://travis-ci.org/msteinert/bstring)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/3174/badge.svg)](https://scan.coverity.com/projects/3174)

# Better String Library

This is a fork of Paul Hsieh's [Better String Library][1]. The following
features (or mis-features, depending on your point of view) are included:

1. Autotools build system
2. Updated test suite based on [Check][2]
3. Add [Valgrind][3] to the workflow
4. Add continuous integration via [Travis-CI][4]
5. Remove C++ code
6. Other various improvements

Currently this fork should be binary-compatible with the original code. The
only source incompatibility is the removal of the `const_bstring` type.

## Building

Autotools is required to build from the source repository.

    $ autoreconf -i
    $ ./configure
    $ make
    $ make install

A test suite is available if [Check][2] is is installed.

    $ make check

If [Valgrind][3] is installed the test suite can be checked for memory leaks.

    $ make memcheck

## Documentation

The original documentation has been migrated into the header files and
converted to the Doxygen format. The generated output can be found
[online][5].

The original documentation essays are available in the source distribution
and are included in the Doxygen output.

1. [Introduction](doc/introduction.md)
2. [Comparisons](doc/comparisons.md)
3. [Porting](doc/porting.md)
4. [Security](doc/security.md)

These essays have been reformatted for easier reading. Minor edits have been
made to reflect changes to the codebase.

## License

The Better String Library is released under the [BSD 3-Clause License][6].

[1]: http://bstring.sourceforge.net/
[2]: http://check.sourceforge.net/
[3]: http://valgrind.org/
[4]: https://travis-ci.org/msteinert/bstring
[5]: http://mike.steinert.ca/bstring/doc/
[6]: http://opensource.org/licenses/BSD-3-Clause
