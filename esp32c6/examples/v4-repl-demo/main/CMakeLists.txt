# Main component for v4-repl-demo example

idf_component_register(
  SRCS
  "main.c"
  INCLUDE_DIRS
  "."
  REQUIRES
  v4_hal_esp32c6
  v4_core
  v4_front)
