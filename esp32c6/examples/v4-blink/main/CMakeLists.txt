# Main component for v4-blink example

idf_component_register(
  SRCS
  "main.c"
  INCLUDE_DIRS
  "."
  REQUIRES
  v4_hal
  v4_core)
