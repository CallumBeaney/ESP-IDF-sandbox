# binds internals of IDF to current project, defines name etc.

# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# to add an external components folder, do below noting forward slash:
# set(EXTRA_COMPONENT_DIRS <path to dir>/dir)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sandbox)
