project('bstring', 'c', version: '1.0.0', default_options: ['warning_level=2'])
cc = meson.get_compiler('c')
pkg = import('pkgconfig')

add_project_arguments('-pedantic', language: 'c')
add_project_arguments('-Wstrict-prototypes', language: 'c')
add_project_arguments('-Wcast-align', language: 'c')
add_project_arguments('-Wno-gnu', language: 'c')
add_project_arguments('-fno-common', language: 'c')
add_project_arguments('-fvisibility=hidden', language: 'c')

bstring_inc = include_directories(['.', 'bstring'])

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
