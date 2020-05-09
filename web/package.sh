#!/bin/bash

emcc liblas_wrapper.cpp \
    -I../LASlib/inc \
    -I../LASzip/src \
    -L../LASlib/lib \
    -llas \
    -s EXPORTED_FUNCTIONS="["\
"'_open_las',"\
"'_close_las',"\
"'_load_points',"\
"'_get_num_points',"\
"'_get_num_loaded_points',"\
"'_get_bounds',"\
"'_get_positions',"\
"'_get_colors',"\
"'_has_colors']"\
    -s ALLOW_MEMORY_GROWTH=1 \
    -s FORCE_FILESYSTEM=1 \
    -s EXTRA_EXPORTED_RUNTIME_METHODS="['FS', 'cwrap']" \
    -o liblas.js


