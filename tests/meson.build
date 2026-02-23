test_executable = executable(
    'bstest',
    'bstest.c',
    link_with: libbstring,
    include_directories: bstring_inc,
    dependencies: check,
)
test_executable_aux = executable(
    'testaux',
    'testaux.c',
    link_with: libbstring,
    include_directories: bstring_inc,
    dependencies: check,
)

test('bstring unit tests', test_executable)
test('bstring auxiliary unit tests', test_executable_aux)

if get_option('enable-utf8')
    test_executable_utf8 = executable(
        'testutf8',
        'testutf8.c',
        link_with: libbstring,
        include_directories: bstring_inc,
        dependencies: check,
    )

    test('bstring UTF-8 unit tests', test_executable_utf8)
endif
