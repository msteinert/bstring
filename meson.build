project(
    'bstring',
    'c',
    version: '1.0.3',
    default_options: ['warning_level=3', 'c_std=c99']
)
cc = meson.get_compiler('c')
pkg = import('pkgconfig')
warning_flags = []

foreach flag: [
    '-fno-common',
    '-fvisibility=hidden',
    '-Wstrict-prototypes',
    '-Wcast-align',
]
    if cc.has_argument(flag)
        warning_flags += flag
    endif
endforeach

if get_option('enable-tests') and cc.has_argument('-Wno-gnu')
    warning_flags += '-Wno-gnu'
endif

if cc.get_id() == 'msvc'
    foreach flag: [
        '/we4431',
        '/w14826',
    ]
        if cc.has_argument(flag)
            warning_flags += flag
        endif
    endforeach
endif

add_project_arguments(warning_flags, language: 'c')
bstring_inc = include_directories(['.', 'bstring'])
conf_data = configuration_data()

if get_option('enable-bgets-workaround')
    conf_data.set('HAVE_BGETS', '1')
else
    conf_data.set('HAVE_BGETS', '0')
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
