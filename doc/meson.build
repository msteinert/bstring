doxyfile_conf = configuration_data()
doxyfile_conf.set('PROJECT_VERSION', meson.project_version())
doxyfile_conf.set('PACKAGE', meson.project_name())
doxyfile_conf.set('VERSION', meson.project_version())
doxyfile_conf.set('ABS_TOP_SRCDIR', meson.project_source_root())
doxyfile_conf.set('OUTPUT_DIR', meson.current_build_dir())

doxyfile = configure_file(input: 'Doxyfile.in', output: 'Doxyfile', configuration: doxyfile_conf)

run_command(doxygen, doxyfile, check: true)

install_subdir(
    meson.current_build_dir() / 'html',
    install_dir: get_option('datadir') / 'doc' / meson.project_name(),
    strip_directory: false,
)
