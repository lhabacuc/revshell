# Makefile for Ubuntu/Debian Package Installer

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
TARGET = installer
SOURCE = installer.cpp

# Default target
all: $(TARGET)

# Build the installer
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Install the installer to user's local bin directory
install: $(TARGET)
	mkdir -p ~/.local/bin
	cp $(TARGET) ~/.local/bin/
	chmod +x ~/.local/bin/$(TARGET)
	@echo "Installer installed to ~/.local/bin/$(TARGET)"
	@echo "Make sure ~/.local/bin is in your PATH"

# Uninstall from user's local bin directory
uninstall:
	rm -f ~/.local/bin/$(TARGET)
	@echo "Installer removed from ~/.local/bin/$(TARGET)"

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the installer (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install to ~/.local/bin/"
	@echo "  uninstall - Remove from ~/.local/bin/"
	@echo "  help      - Show this help message"

.PHONY: all clean install uninstall help