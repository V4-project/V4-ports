# Main component for v4-repl-demo example

idf_component_register(
  SRCS
  "main.c"
  INCLUDE_DIRS
  "."
  REQUIRES
  v4_hal
  v4_core
  v4_front
  v4_repl
  PRIV_REQUIRES
  esp_driver_usb_serial_jtag
  vfs)
