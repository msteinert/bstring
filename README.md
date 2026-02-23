# Better String Library

[![SonarQube Security](https://sonarcloud.io/api/project_badges/measure?project=msteinert_bstring&metric=security_rating)](https://sonarcloud.io/project/overview?id=msteinert_bstring)
[![SonarQube Reliability](https://sonarcloud.io/api/project_badges/measure?project=msteinert_bstring&metric=reliability_rating)](https://sonarcloud.io/project/overview?id=msteinert_bstring)
[![SonarQube Maintainability](https://sonarcloud.io/api/project_badges/measure?project=msteinert_bstring&metric=sqale_rating)](https://sonarcloud.io/project/overview?id=msteinert_bstring)
[![OpenSSF Scorecard](https://img.shields.io/ossf-scorecard/github.com/msteinert/bstring?label=openssf+scorecard&style=flat)](https://scorecard.dev/viewer/?uri=github.com/msteinert/bstring)

The Better String Library is an abstraction of a string data type which is
superior to the C standard library char buffer string type.
Among the features achieved are:

- Substantial mitigation of buffer overflow/overrun problems and other
  failures that result from erroneous usage of the common C string
  library functions
- Significantly simplified string manipulation
- High performance interoperability with other source/libraries which
  expect '\0' terminated char buffers
- Improved overall performance of common string operations
- Functional equivalency with other more modern languages
- Optional API for manipulating UTF-8 encoded strings

## bstring fork

This is a fork of Paul Hsieh's [Better String Library][]. The following
features (or mis-features, depending on your point of view) are included:

1. A build system ([Meson][]+[Ninja][])
2. Improved test suite using the [Check][] library
3. Continuous integration via GitHub Actions, including memory profiling with [Valgrind][]
4. Remove C++ wrapper code, returning this to a pure C library
5. Documentation generation with [Doxygen][]

Currently this fork should be binary-compatible with the original code. The
only source incompatibility is the removal of the `const_bstring` type.
Just use `const bstring` instead.

[Better String Library]: http://bstring.sourceforge.net/
[Meson]: https://mesonbuild.com/
[Ninja]: https://ninja-build.org/
[Check]: https://github.com/libcheck/check
[Valgrind]: http://valgrind.org/
[Doxygen]: https://www.doxygen.nl/

## Get bstring

Another advantage of this fork is that unlike the original, it is widely available as a binary package in various distributions.
The shared library and development headers can be installed with their respective package manager.

The current packaging status as reported by repology.org:

[![Packaging status](https://repology.org/badge/vertical-allrepos/bstring.svg)](https://repology.org/project/bstring/versions)

## Building

The bstring library can be built using the [Meson][]+[Ninja][] build system.

The library itself doesn't have any dependencies beyond a C compiler,
but the test suite requires the [Check][] unit testing framework,
while the documentation generation requires [Doxygen][].

Configure the `build` directory with Meson.

    meson setup build

Alternatively, enable building the documentation and test suite.

    meson setup build -Denable-docs=true -Denable-tests=true

Then compile and install.

    meson compile -C build
    sudo meson install -C build

A test suite is available if Check is is installed.

    meson test -C build

If Valgrind is installed the test suite can be checked for memory leaks.

    meson test --wrapper='valgrind --leak-check=full' -C build

## Documentation

The original documentation has been migrated into the header files and
converted to the Doxygen format. The generated output can be found
[online][].

The original documentation essays are available in the source distribution
and are included in the Doxygen output.

1. [Introduction](doc/introduction.md)
2. [Comparisons](doc/comparisons.md)
3. [Porting](doc/porting.md)
4. [Security](doc/security.md)

These essays have been reformatted for easier reading. Minor edits have been
made to reflect changes to the codebase.

[online]: http://mike.steinert.ca/bstring/doc/

## License

The Better String Library is released under the [BSD 3-Clause License][]
or the [GNU General Public License v2][] at the option of the user.

[BSD 3-Clause License]: http://opensource.org/licenses/BSD-3-Clause
[GNU General Public License v2]: https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
