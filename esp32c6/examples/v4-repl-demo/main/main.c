/**
 * @file main.c
 * @brief V4 REPL Demo for ESP32-C6
 *
 * Interactive REPL (Read-Eval-Print Loop) for V4 VM over USB Serial/JTAG.
 * Allows executing Forth code interactively via serial console.
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "driver/usb_serial_jtag.h"
#include "driver/usb_serial_jtag_vfs.h"
#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "v4/hal.h"
#include "v4/vm_api.h"
#include "v4front/compile.h"

// VM configuration
#define ARENA_SIZE (16 * 1024)  // 16KB arena for VM

// REPL configuration
#define REPL_PROMPT "v4> "
#define MAX_LINE_LENGTH 256

// LED configuration
#define LED_GPIO 7  // GPIO7 for LED

static uint8_t arena_buf[ARENA_SIZE];

/**
 * @brief Print welcome banner
 */
static void print_banner(void)
{
  printf("\n");
  printf("========================================\n");
  printf("V4 REPL Demo - ESP32-C6\n");
  printf("========================================\n");
  printf("Forth Interactive Shell\n");
  printf("Type Forth code and press Enter\n");
  printf("Arena: %d bytes\n", ARENA_SIZE);
  printf("Stack: 16KB (optimized via v4_front)\n");
  printf("Console: USB Serial/JTAG\n");
  printf("System: ESP32-C6 with V4-hal\n");  // TODO: Add system_info to HAL API
  printf("LED: GPIO%d\n", LED_GPIO);
  printf("========================================\n\n");
}

/**
 * @brief Set LED state - Native Forth word for "n led!" syntax
 * Stack effect: ( n -- )
 * Takes 0 or non-zero value from stack
 */
static int led_set_impl(struct Vm *vm)
{
  v4_i32 value;
  int err = vm_ds_pop(vm, &value);
  if (err != 0)
  {
    return err;
  }

  // Set LED based on value (0=off, non-zero=on)
  hal_gpio_write(LED_GPIO, (value != 0) ? HAL_GPIO_HIGH : HAL_GPIO_LOW);
  return 0;
}

/**
 * @brief Process and execute Forth code line
 */
static void process_line(struct Vm *vm, V4FrontContext *ctx, const char *line)
{
  if (strlen(line) == 0)
  {
    return;
  }

  // Handle "n led!" pattern with special parsing
  if (strstr(line, "led!") != NULL)
  {
    int value;
    if (sscanf(line, "%d led!", &value) == 1)
    {
      v4_i32 val = (v4_i32)value;
      vm_ds_push(vm, val);
      led_set_impl(vm);
      printf("ok\n");
      return;
    }
  }

  // Compile Forth source with new API
  V4FrontBuf buf = {0};
  V4FrontError error = {0};
  v4front_err err = v4front_compile_with_context_ex(ctx, line, &buf, &error);

  if (err != 0)
  {
    // Compilation error - format with position information
    char error_buf[512];
    v4front_format_error(&error, line, error_buf, sizeof(error_buf));
    printf("%s", error_buf);  // error_buf already contains newlines
    return;
  }

  // Register compiled words to VM and compiler context
  for (int i = 0; i < buf.word_count; i++)
  {
    V4FrontWord *word = &buf.words[i];

    // Register to VM
    int wid = vm_register_word(vm, word->name, word->code, (int)word->code_len);
    if (wid < 0)
    {
      printf("ERROR: Failed to register word '%s' (code %d)\n", word->name, wid);
      v4front_free(&buf);
      return;
    }

    // Register to compiler context so it can be used in subsequent compilations
    v4front_err ctx_err = v4front_context_register_word(ctx, word->name, wid);
    if (ctx_err != 0)
    {
      printf("ERROR: Failed to register word '%s' to compiler (code %ld)\n", word->name, (long)ctx_err);
      v4front_free(&buf);
      return;
    }
  }

  // Execute immediate code if any
  if (buf.data && buf.size > 0)
  {
    int wid = vm_register_word(vm, NULL, buf.data, (int)buf.size);
    struct Word *entry = vm_get_word(vm, wid);
    v4_err vm_err = vm_exec(vm, entry);

    if (vm_err != 0)
    {
      printf("ERROR: VM execution failed (code %d)\n", vm_err);
    }
    else
    {
      // Print stack top if available
      int depth = vm_ds_depth_public(vm);
      if (depth > 0)
      {
        v4_i32 top;
        vm_ds_pop(vm, &top);
        printf(" => %ld (0x%08lX)\n", (long)top, (unsigned long)top);
      }
      else
      {
        printf("ok\n");
      }
    }
  }
  else
  {
    printf("ok\n");
  }

  v4front_free(&buf);
}

/**
 * @brief Read a line from stdin with simple echo
 */
static int read_line(char *buffer, size_t max_len)
{
  size_t pos = 0;

  while (pos < max_len - 1)
  {
    int c = getchar();

    if (c == EOF || c < 0)
    {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    // Handle backspace
    if (c == '\b' || c == 127)
    {
      if (pos > 0)
      {
        pos--;
        printf("\b \b");  // Erase character on screen
        fflush(stdout);
      }
      continue;
    }

    // Handle carriage return or newline
    if (c == '\r' || c == '\n')
    {
      buffer[pos] = '\0';
      printf("\n");
      fflush(stdout);
      return pos;
    }

    // Ignore control characters (except handled above)
    if (c < 32)
    {
      continue;
    }

    // Echo and store character
    buffer[pos++] = (char)c;
    putchar(c);
    fflush(stdout);
  }

  buffer[pos] = '\0';
  return pos;
}

/**
 * @brief Main application entry point
 */
void app_main(void)
{
  // Configure USB Serial/JTAG for non-blocking REPL
  // Based on: https://www.esp32.com/viewtopic.php?t=27944

  // Disable buffering on stdin and stdout
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  // Install USB Serial/JTAG driver for interrupt-driven reads and writes
  usb_serial_jtag_driver_config_t usb_serial_jtag_config = {
      .rx_buffer_size = 1024,
      .tx_buffer_size = 1024,
  };
  usb_serial_jtag_driver_install(&usb_serial_jtag_config);

  // Configure line endings for terminal compatibility
  usb_serial_jtag_vfs_set_rx_line_endings(ESP_LINE_ENDINGS_CR);
  usb_serial_jtag_vfs_set_tx_line_endings(ESP_LINE_ENDINGS_CRLF);

  // Use the driver for VFS (required for non-blocking I/O)
  usb_serial_jtag_vfs_use_driver();

  // Set non-blocking mode on stdin
  int flags = fcntl(fileno(stdin), F_GETFL, 0);
  fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);

  // Wait longer for USB enumeration to complete
  // This prevents banner corruption at startup
  vTaskDelay(pdMS_TO_TICKS(500));

  // Clear any garbage in the buffer
  printf("\n\n\n");
  fflush(stdout);
  vTaskDelay(pdMS_TO_TICKS(100));

  print_banner();

  // Create VM with arena allocator
  VmConfig config = {
      .mem = arena_buf,
      .mem_size = ARENA_SIZE,
      .mmio = NULL,
      .mmio_count = 0,
      .arena = NULL,
  };

  struct Vm *vm = vm_create(&config);
  if (!vm)
  {
    printf("ERROR: Failed to create VM\n");
    return;
  }
  printf("VM created successfully\n");

  // Initialize LED GPIO
  int gpio_err = hal_gpio_mode(LED_GPIO, HAL_GPIO_OUTPUT);
  if (gpio_err != 0)
  {
    printf("ERROR: Failed to initialize LED GPIO%d\n", LED_GPIO);
  }
  else
  {
    // Turn off LED initially
    hal_gpio_write(LED_GPIO, HAL_GPIO_LOW);
    printf("LED GPIO%d initialized\n", LED_GPIO);
  }

  // Register LED control native words
  // TODO: Need to find correct API for registering native functions
  // For now, document the available words

  // Create V4-front compiler context
  V4FrontContext *ctx = v4front_context_create();
  if (!ctx)
  {
    printf("ERROR: Failed to create compiler context\n");
    return;
  }
  printf("Compiler context created\n");

  // Register LED control functions as V4 words using SYS instructions
  // led-on: 7 1 0x01 SYS RET
  static uint8_t led_on_code[] = {
      0x76, LED_GPIO,  // LIT_U8 7 (GPIO pin number)
      0x74,            // LIT1 (value = 1 = HIGH)
      0x60, 0x01,      // SYS 0x01 (V4_SYS_GPIO_WRITE)
      0x51             // RET
  };

  // led-off: 7 0 0x01 SYS RET
  static uint8_t led_off_code[] = {
      0x76, LED_GPIO,  // LIT_U8 7 (GPIO pin number)
      0x73,            // LIT0 (value = 0 = LOW)
      0x60, 0x01,      // SYS 0x01 (V4_SYS_GPIO_WRITE)
      0x51             // RET
  };

  // led-toggle: Read current GPIO state, invert it, and write back
  // Bytecode: 7 0x02 SYS (GPIO_READ) -> NOT -> 7 SWAP -> 0x01 SYS (GPIO_WRITE) -> RET
  static uint8_t led_toggle_code[] = {
      0x76, LED_GPIO,  // LIT_U8 7 (GPIO pin number)
      0x60, 0x02,      // SYS 0x02 (V4_SYS_GPIO_READ) - reads pin state to stack
      0x2B,            // INVERT (bitwise NOT)
      0x76, LED_GPIO,  // LIT_U8 7 (GPIO pin number again)
      0x03,            // SWAP (pin, inverted_value)
      0x60, 0x01,      // SYS 0x01 (V4_SYS_GPIO_WRITE)
      0x51             // RET
  };

  int wid_on = vm_register_word(vm, "led-on", led_on_code, sizeof(led_on_code));
  int wid_off = vm_register_word(vm, "led-off", led_off_code, sizeof(led_off_code));
  int wid_toggle = vm_register_word(vm, "led-toggle", led_toggle_code, sizeof(led_toggle_code));

  if (wid_on >= 0 && wid_off >= 0 && wid_toggle >= 0)
  {
    // Register to compiler context so they can be used in definitions
    v4front_context_register_word(ctx, "led-on", wid_on);
    v4front_context_register_word(ctx, "led-off", wid_off);
    v4front_context_register_word(ctx, "led-toggle", wid_toggle);
    printf("LED control words registered\n\n");
  }
  else
  {
    printf("WARNING: Failed to register LED words\n\n");
  }

  printf("Available LED commands:\n");
  printf("  led-on     - Turn LED on\n");
  printf("  led-off    - Turn LED off\n");
  printf("  led-toggle - Toggle LED state\n");
  printf("  n led!     - Set LED (0=off, non-zero=on)\n");
  printf("\nYou can now use these in word definitions and control structures:\n");
  printf("  : blink led-on led-off ;\n");
  printf("  1 if led-on then\n\n");

  // Main REPL loop
  char line[MAX_LINE_LENGTH];
  while (1)
  {
    // Print prompt
    printf("%s", REPL_PROMPT);
    fflush(stdout);

    // Read line
    int len = read_line(line, MAX_LINE_LENGTH);

    // Skip empty lines
    if (len == 0)
    {
      continue;
    }

    // Process the line
    process_line(vm, ctx, line);
  }

  // Cleanup (unreachable in this implementation)
  printf("\nExiting V4 REPL\n");
  v4front_context_destroy(ctx);
  vm_destroy(vm);
}
