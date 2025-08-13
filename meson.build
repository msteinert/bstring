project('bstring', 'c', version: '1.0.1', default_options: ['warning_level=2'])
cc = meson.get_compiler('c')
pkg = import('pkgconfig')

add_project_arguments('-pedantic', language: 'c')
add_project_arguments('-Wstrict-prototypes', language: 'c')
add_project_arguments('-Wcast-align', language: 'c')
add_project_arguments('-Wno-gnu', language: 'c')
add_project_arguments('-fno-common', language: 'c')
add_project_arguments('-fvisibility=hidden', language: 'c')

bstring_inc = include_directories(['.', 'bstring'])
bstring_deps = []
libc_dep = cc.find_library('c', required: false)

bgets_test_code = '''
#include <stdio.h>
#include <libgen.h>
int main() {
    char buffer[256];
    char delim = '\n';
    FILE *fp = stdin;
    char *result = bgets(buffer, sizeof(buffer), fp, &delim);
    return 0;
}
'''

has_bgets = cc.compiles(bgets_test_code, name: 'bgets defined in standard C library')
conf_data = configuration_data()

if has_bgets or get_option('force-bgets-workaround')
    conf_data.set('HAVE_LIBGEN_H_BGETS', '1')
else
    conf_data.set('HAVE_LIBGEN_H_BGETS', '0')
endif

configure_file(
    output: 'config.h',
    configuration: conf_data
)

subdir('bstring')

pkg.generate(
    libbstring,
    name: meson.project_name(),
    description: 'The Better String Library',
    version: meson.project_version(),
    subdirs: ['bstring'],
)

check = dependency('check', required: false)

if get_option('enable-tests')
    if check.found()
        subdir('tests')
    else
        error('Tests requested but check library not found')
    endif
endif

doxygen = find_program('doxygen', required: false)

if get_option('enable-docs')
    if doxygen.found()
        subdir('doc')
    else
        error('Docs requested but doxygen not found')
    endif
endif
