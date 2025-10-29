.PHONY: all format format-check clean help

# Default target
all: help

# Apply formatting
format:
	@echo "✨ Formatting C/C++ code..."
	@find esp32c6 -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) \
		-not -path "*/build/*" -exec clang-format -i {} \;
	@echo "✨ Formatting CMake files..."
	@find esp32c6 -name 'CMakeLists.txt' -o -name '*.cmake' | grep -v '/build/' | xargs cmake-format -i
	@echo "✅ Formatting complete!"

# Format check (for CI)
format-check:
	@echo "🔍 Checking C/C++ formatting..."
	@find esp32c6 -type f \( -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) \
		-not -path "*/build/*" | xargs clang-format --dry-run --Werror || \
		(echo "❌ C/C++ formatting check failed. Run 'make format' to fix." && exit 1)
	@echo "🔍 Checking CMake formatting..."
	@find esp32c6 -name 'CMakeLists.txt' -o -name '*.cmake' | grep -v '/build/' | xargs cmake-format --check || \
		(echo "❌ CMake formatting check failed. Run 'make format' to fix." && exit 1)
	@echo "✅ All formatting checks passed!"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	@find esp32c6 -type d -name 'build' -exec rm -rf {} + 2>/dev/null || true
	@find esp32c6 -type f \( -name 'sdkconfig' -o -name 'sdkconfig.old' \) -exec rm -f {} + 2>/dev/null || true
	@echo "✅ Clean complete!"

# Build examples (using Docker)
build-docker:
	@echo "🔨 Building examples in Docker..."
	@docker compose run --rm esp-idf bash -c "\
		cd esp32c6/examples/v4-blink && idf.py build && \
		echo '✅ v4-blink built successfully'"

# Help
help:
	@echo "V4-ports Makefile targets:"
	@echo ""
	@echo "  make format          - Format code with clang-format and cmake-format"
	@echo "  make format-check    - Check formatting without modifying files (for CI)"
	@echo "  make clean           - Remove build artifacts"
	@echo "  make build-docker    - Build examples using Docker"
	@echo "  make help            - Show this help message"
	@echo ""
	@echo "ESP-IDF Build (Native):"
	@echo "  cd esp32c6/examples/v4-blink"
	@echo "  idf.py build                    # Build"
	@echo "  idf.py flash                    # Flash to device"
	@echo "  idf.py monitor                  # Serial monitor"
	@echo "  idf.py flash monitor            # Flash and monitor"
	@echo ""
	@echo "ESP-IDF Build (Docker):"
	@echo "  docker compose run --rm esp-idf"
	@echo "  cd esp32c6/examples/v4-blink"
	@echo "  idf.py build"
	@echo ""
