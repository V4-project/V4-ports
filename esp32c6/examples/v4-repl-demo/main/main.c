/**
 * @file main.c
 * @brief V4 REPL Demo for ESP32-C6
 *
 * Interactive REPL (Read-Eval-Print Loop) for V4 VM over UART.
 * Allows executing Forth code interactively via serial console.
 */

#include "v4/v4_hal.h"
#include "v4/vm_api.h"
#include "v4front/compile.h"
#include <stdio.h>
#include <string.h>

// UART configuration
#define UART_PORT 0
#define UART_BAUDRATE 115200

// Line buffer configuration
#define LINE_BUF_SIZE 256

// VM configuration
#define ARENA_SIZE (16 * 1024) // 16KB arena for VM

static char line_buf[LINE_BUF_SIZE];
static int line_pos = 0;
static uint8_t arena_buf[ARENA_SIZE];

/**
 * @brief Print welcome banner
 */
static void print_banner(void) {
    printf("\n");
    printf("========================================\n");
    printf("V4 REPL Demo - ESP32-C6\n");
    printf("========================================\n");
    printf("Forth Interactive Shell\n");
    printf("Type Forth code and press Enter\n");
    printf("Arena Size: %d bytes\n", ARENA_SIZE);
    printf("========================================\n\n");
}

/**
 * @brief Process and execute Forth code line
 */
static void process_line(VM *vm, V4FrontContext *ctx, const char *line) {
    if (strlen(line) == 0) {
        return;
    }

    printf("\n");

    // Compile Forth source
    V4FrontCompileOutput *output = v4front_compile_with_context(line, ctx);

    if (output->error_code != 0) {
        // Compilation error
        printf("ERROR: %s\n", output->error_message);
        v4front_free(output);
        return;
    }

    // Register compiled words to VM
    for (int i = 0; i < output->word_count; i++) {
        V4Word *word = &output->words[i];
        vm_register_word(vm, word->name, word->bytecode, word->bytecode_len);
    }

    // Execute the last word if any
    if (output->word_count > 0) {
        V4Word *last_word = &output->words[output->word_count - 1];
        Err vm_err = vm_call(vm, vm_get_word(vm, last_word->name));

        if (vm_err != 0) {
            printf("ERROR: VM execution failed (code %d)\n", vm_err);
        } else {
            // Print stack top if available
            if (vm_ds_depth_public(vm) > 0) {
                int32_t top = vm_ds_pop(vm);
                printf(" => %d (0x%08X)\n", top, (unsigned)top);
            } else {
                printf("ok\n");
            }
        }
    }

    v4front_free(output);
}

/**
 * @brief Handle backspace character
 */
static void handle_backspace(void) {
    if (line_pos > 0) {
        line_pos--;
        // Send backspace sequence: BS + space + BS
        printf("\b \b");
        fflush(stdout);
    }
}

/**
 * @brief Main application entry point
 */
void app_main(void) {
    print_banner();

    // Initialize UART
    v4_err err = v4_hal_uart_init(UART_PORT, UART_BAUDRATE);
    if (err != V4_ERR_OK) {
        printf("ERROR: Failed to initialize UART (error %d)\n", err);
        return;
    }
    printf("UART initialized: %d baud\n\n", UART_BAUDRATE);

    // Create VM with arena allocator
    VmConfig config = {
        .arena_buffer = arena_buf,
        .arena_size = ARENA_SIZE,
    };

    VM *vm = vm_create(&config);
    if (!vm) {
        printf("ERROR: Failed to create VM\n");
        return;
    }
    printf("VM created successfully\n");

    // Create V4-front compiler context
    V4FrontContext *ctx = v4front_context_create();
    if (!ctx) {
        printf("ERROR: Failed to create compiler context\n");
        return;
    }
    printf("Compiler context created\n\n");

    // Print prompt
    printf("v4> ");
    fflush(stdout);

    // Main REPL loop
    while (1) {
        // Check for available UART data
        int available;
        err = v4_hal_uart_available(UART_PORT, &available);

        if (err == V4_ERR_OK && available > 0) {
            char c;
            int read_len;
            err = v4_hal_uart_read(UART_PORT, &c, 1, &read_len);

            if (err == V4_ERR_OK && read_len > 0) {
                // Handle newline (Enter key)
                if (c == '\n' || c == '\r') {
                    line_buf[line_pos] = '\0';

                    // Process the line
                    if (line_pos > 0) {
                        process_line(vm, ctx, line_buf);
                    }

                    // Reset line buffer
                    line_pos = 0;

                    // Print new prompt
                    printf("v4> ");
                    fflush(stdout);
                }
                // Handle backspace/delete
                else if (c == 0x08 || c == 0x7F) {
                    handle_backspace();
                }
                // Handle Ctrl+C (reset line)
                else if (c == 0x03) {
                    line_pos = 0;
                    printf("^C\nv4> ");
                    fflush(stdout);
                }
                // Handle normal characters
                else if (c >= 0x20 && c < 0x7F) {
                    if (line_pos < LINE_BUF_SIZE - 1) {
                        line_buf[line_pos++] = c;
                        // Echo character back
                        v4_hal_uart_write(UART_PORT, &c, 1);
                    }
                }
            }
        }

        // Small delay to reduce CPU usage
        v4_hal_delay_ms(10);
    }
}
