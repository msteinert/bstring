[![Coverity Scan Build Status](https://scan.coverity.com/projects/3174/badge.svg)](https://scan.coverity.com/projects/3174)

# Better String Library

This is a fork of Paul Hsieh's [Better String Library][]. The following
features (or mis-features, depending on your point of view) are included:

1. Autotools build system
2. Updated test suite based on [Check][2]
3. Add [Valgrind][] to the workflow
4. Add continuous integration via GitHub Actions
5. Remove C++ code
6. Other various improvements

Currently this fork should be binary-compatible with the original code. The
only source incompatibility is the removal of the `const_bstring` type.

[Better String Library]: http://bstring.sourceforge.net/
[Check]: http://check.sourceforge.net/
[Valgrind]: http://valgrind.org/

## Building

The repository currently includes [Meson][] and Autotools ([Autoconf][],
[Automake][], and [Libtool][]) build systems.

[Meson]: https://mesonbuild.com/
[Autoconf]: https://www.gnu.org/software/autoconf/
[Automake]: https://www.gnu.org/software/automake/
[Libtool]: https://www.gnu.org/software/libtool/

### Meson

To build with Meson:

    $ meson setup build -Denable-docs=true -Denable-tests=true
    $ meson compile -C build
    $ sudo meson install -C build

A test suite is available if Check is is installed.

    $ meson test -C build

### Autotools

To build with Autotools:

    $ autoreconf -i
    $ ./configure
    $ make
    $ make install

A test suite is available if Check is is installed.

    $ make check

If Valgrind is installed the test suite can be checked for memory leaks.

    $ make memcheck

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

The Better String Library is released under the [BSD 3-Clause License][].

[BSD 3-Clause License]: http://opensource.org/licenses/BSD-3-Clause
