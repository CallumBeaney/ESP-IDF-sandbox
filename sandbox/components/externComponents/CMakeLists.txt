# Describes files/headers inside the enclosing folder.
# RE: REQUIRES: check ~/esp/esp-idf/components to see which components are needed
# again: you need to run idf.py build to recompile in order to make intellisense behave.
idf_component_register(
  SRCS "extern.c" 
  INCLUDE_DIRS "." 
  REQUIRES "log")
