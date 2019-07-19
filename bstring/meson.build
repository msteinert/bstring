###################################################################################
#                                                                                 #
# NAME: bstring/meson.build                                                       #
#                                                                                 #
# AUTHOR: Paul Hsieh.                                                             #
# WRITTEN BY: Michael Brockus.                                                    #
#                                                                                 #
# License: MIT                                                                    #
#                                                                                 #
###################################################################################



bstring_src = files('bstraux.c', 'bstrlib.c')

bstring_dir = include_directories('.')

bstring_lib = static_library(meson.project_name(), 
    sources: bstring_src,
    include_directories: bstring_dir)