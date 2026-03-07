bstring_sources = ['bstraux.c', 'bstrlib.c']
bstring_headers = ['bstraux.h', 'bstrlib.h']

if get_option('enable-utf8')
    bstring_sources += ['buniutil.c', 'utf8util.c']
    bstring_headers += ['buniutil.h', 'utf8util.h']
endif

install_headers(bstring_headers)

# When fuzzing, the library must be static so that coverage-instrumented object
# files are linked directly into the fuzz binary, where the sanitizer runtime
# can resolve the __sanitizer_cov_* symbols.  A shared library would leave
# those references dangling at library link time.
if get_option('enable-fuzzing')
    libbstring = static_library(
        meson.project_name(),
        bstring_sources,
        include_directories: bstring_inc,
        install: false,
    )
else
    libbstring = library(
        meson.project_name(),
        bstring_sources,
        version: meson.project_version(),
        soversion: '1',
        include_directories: bstring_inc,
        install: true,
    )
endif

bstring_dep = declare_dependency(include_directories: bstring_inc, link_with: libbstring)
