install_headers(['bstraux.h', 'bstrlib.h'])

libbstring = library(
    meson.project_name(),
    ['bstraux.c', 'bstrlib.c'],
    version: '1.0.0',
    soversion: '1',
    include_directories: bstring_inc,
    install: true,
    install_dir: get_option('libdir'),
)

bstring_dep = declare_dependency(include_directories: bstring_inc, link_with: libbstring)
