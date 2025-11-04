# Claude Code Notes

## Repository Status

**⚠️ This repository is DEPRECATED and scheduled for archival.**

- **Replacement**: [V4-runtime](https://github.com/kirisaki/V4-runtime)
- **Status**: Read-only, no new features
- **Migration**: All new development should use V4-runtime

## Important Context for Claude Code

### Why Deprecated?

V4-ports was the initial platform HAL implementation repository, but the architecture has evolved:

1. **Old Architecture** (V4-ports):
   - Monolithic ports per platform
   - HAL implementations tightly coupled with examples
   - Limited FreeRTOS integration

2. **New Architecture** (V4-runtime):
   - Unified runtime environment
   - Better FreeRTOS task backend integration
   - Cleaner separation of concerns
   - V4-engine supports pluggable task backends (CUSTOM/FREERTOS)

### Task Backend Abstraction (2025-11-04)

V4-engine now has a task backend abstraction layer:

- **Custom Backend**: V4's own scheduler (priority + round-robin)
- **FreeRTOS Backend**: Native FreeRTOS integration for ESP32 and other RTOS platforms

Implementation files:
- `V4-engine/src/task_backend_custom.cpp` - Custom scheduler
- `V4-engine/src/task_backend_freertos.cpp` - FreeRTOS integration
- `V4-engine/include/v4/internal/task_backend.h` - Backend interface

### What NOT to Do

❌ **DO NOT** update V4-ports components or examples
❌ **DO NOT** add new features to V4-ports
❌ **DO NOT** migrate V4-ports to use V4-engine's new task backend

### What TO Do Instead

✅ **DO** refer users to V4-runtime for new projects
✅ **DO** update V4-runtime with FreeRTOS backend integration
✅ **DO** maintain this deprecation notice

## Migration Path

For users asking about ESP32/embedded integration:

1. Point them to **V4-runtime** repository
2. V4-runtime will integrate V4-engine with FreeRTOS backend
3. Use `V4_TASK_BACKEND=FREERTOS` in CMake for ESP-IDF builds

## Repository References

- **V4-engine**: https://github.com/V4-project/V4-engine (VM core)
- **V4-front**: https://github.com/kirisaki/V4-front (Forth compiler)
- **V4-runtime**: https://github.com/kirisaki/V4-runtime (Runtime environment)
- **V4-ports**: https://github.com/kirisaki/V4-ports (DEPRECATED)

## Last Updated

2025-11-04: Added deprecation notice and task backend context
