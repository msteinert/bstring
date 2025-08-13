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

if get_option('enable-coverage')
    if cc.get_id() == 'gcc'
        gcov = find_program('gcov', required: true)
        lcov = find_program('lcov', required: true)
        genhtml = find_program('genhtml', required: true)

        run_target('coverage-gcov',
            command: [
            'sh', '-c',
            'cd @0@ && find . -name "*.gcda" -exec @1@ {} \\;'.format(meson.current_build_dir(), gcov.full_path())
            ],
            depends: [test_executable, test_executable_aux])
        run_target('coverage-lcov',
            command: [
            lcov, '--capture', '--directory', meson.current_build_dir(),
            '--output-file', 'coverage.info',
            '--exclude', '*/tests/*'
            ],
            depends: [test_executable, test_executable_aux])
        run_target('coverage-html',
            command : [genhtml, '--output-directory', meson.current_build_dir(), 'coverage.info'],
            depends : [test_executable, test_executable_aux])
    else
        error('Code coverage is only supported with gcc')
    endif
endif
