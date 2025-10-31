# Main component for v4-link-demo example

idf_component_register(
  SRCS
  "main.cpp"
  INCLUDE_DIRS
  "."
  REQUIRES
  v4_hal
  v4_core
  v4_link)
