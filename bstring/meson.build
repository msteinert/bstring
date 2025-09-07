install_headers(['bstraux.h', 'bstrlib.h'])

libbstring = library(
    meson.project_name(),
    ['bstraux.c', 'bstrlib.c'],
    version: meson.project_version(),
    soversion: '1',
    include_directories: bstring_inc,
    install: true,
)

bstring_dep = declare_dependency(include_directories: bstring_inc, link_with: libbstring)
