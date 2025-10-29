#include <stddef.h>

#include "v4repl/repl.h"

/* Ultra-minimal local stub - no external dependencies */

V4ReplContext* v4_repl_create(const V4ReplConfig* config)
{
  (void)config;
  return NULL;
}

void v4_repl_destroy(V4ReplContext* ctx)
{
  (void)ctx;
}

v4_err v4_repl_process_line(V4ReplContext* ctx, const char* line)
{
  (void)ctx;
  (void)line;
  return 0;
}

void v4_repl_reset(V4ReplContext* ctx)
{
  (void)ctx;
}

void v4_repl_reset_dictionary(V4ReplContext* ctx)
{
  (void)ctx;
}

int v4_repl_stack_depth(const V4ReplContext* ctx)
{
  (void)ctx;
  return 0;
}

void v4_repl_print_stack(const V4ReplContext* ctx)
{
  (void)ctx;
}

const char* v4_repl_get_error(const V4ReplContext* ctx)
{
  (void)ctx;
  return NULL;
}

int v4_repl_version(void)
{
  return 0x000100;
}
