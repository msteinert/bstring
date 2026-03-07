fuzzer_c_args = []
fuzzer_link_arg = get_option('fuzz-link-arg')
fuzzer_link_args = [fuzzer_link_arg]

if get_option('enable-utf8')
    fuzzer_c_args += '-DBSTRING_ENABLE_UTF8'
endif

executable(
    'fuzz_bstring',
    'fuzz_bstring.c',
    link_with: libbstring,
    include_directories: bstring_inc,
    c_args: fuzzer_c_args,
    link_args: fuzzer_link_args,
)
