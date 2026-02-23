bstring_sources = ['bstraux.c', 'bstrlib.c']
bstring_headers = ['bstraux.h', 'bstrlib.h']

if get_option('enable-utf8')
    bstring_sources += ['buniutil.c', 'utf8util.c']
    bstring_headers += ['buniutil.h', 'utf8util.h']
endif

install_headers(bstring_headers)

libbstring = library(
    meson.project_name(),
    bstring_sources,
    version: meson.project_version(),
    soversion: '1',
    include_directories: bstring_inc,
    install: true,
)

bstring_dep = declare_dependency(include_directories: bstring_inc, link_with: libbstring)
