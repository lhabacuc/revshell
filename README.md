# Ubuntu/Debian Package Installer

A C++ command-line tool for installing various types of packages and files on Ubuntu/Debian systems at the user level.

## Features

- **User-level installation**: Installs packages to user directories without requiring root privileges (where possible)
- **Multiple file type support**:
  - `.deb` packages (Debian/Ubuntu packages)
  - `.AppImage` files (Portable applications)
  - Binary executables
  - Libraries (`.so`, `.a` files)
- **Automatic type detection**: Automatically detects file type and uses appropriate installation method
- **Manual type override**: Force installation as specific type using command-line options

## Installation

### Prerequisites

- C++ compiler with C++20 support (g++ 10+)
- make utility

### Building from source

```bash
git clone <repository-url>
cd revshell
make
```

### Installing the installer

```bash
make install
```

This will install the `installer` binary to `~/.local/bin/`. Make sure this directory is in your PATH:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

Add this line to your `~/.bashrc` or `~/.zshrc` to make it permanent.

## Usage

### Basic usage

```bash
installer <file1> [file2] [file3] ...
```

### Options

- `-h, --help`: Show help message
- `-d, --deb`: Force install as .deb package
- `-a, --appimage`: Force install as AppImage
- `-b, --binary`: Force install as binary
- `-l, --library`: Force install as library
- `-v, --verbose`: Enable verbose output

### Examples

```bash
# Install a single .deb package
installer package.deb

# Install an AppImage
installer MyApp.AppImage

# Install multiple files with verbose output
installer -v binary1 library.so app.AppImage

# Force install a file as binary
installer -b somefile

# Install all files in current directory (be careful!)
installer *
```

## Installation Locations

The installer places files in the following user-local directories:

- **Binaries**: `~/.local/bin/`
- **Libraries**: `~/.local/lib/`
- **AppImages**: `~/.local/share/appimages/`
- **Deb packages**: Attempts user-local installation first, falls back to system-wide (requires sudo)

## File Type Detection

The installer automatically detects file types based on:

1. **`.deb` packages**: Files ending with `.deb`
2. **AppImages**: Files ending with `.AppImage` or `.appimage`
3. **Libraries**: Files ending with `.so`, containing `.so.`, or ending with `.a`
4. **Binaries**: Executable files that don't match other categories

## .deb Package Installation

For `.deb` packages, the installer:

1. First attempts user-level installation using `dpkg --force-not-root`
2. If that fails, falls back to system-wide installation using `sudo dpkg -i`

**Note**: System-wide installation requires sudo privileges.

## AppImage Installation

AppImages are:

1. Copied to `~/.local/share/appimages/`
2. Made executable
3. Symlinked to `~/.local/bin/` for easy command-line access

## Binary Installation

Executable files are:

1. Copied to `~/.local/bin/`
2. Made executable
3. Ready to run (if `~/.local/bin` is in PATH)

## Library Installation

Library files are copied to `~/.local/lib/` for use by other applications.

## Troubleshooting

### Permission denied errors

- Ensure the installer has execute permissions: `chmod +x installer`
- For .deb packages requiring system installation, ensure you have sudo privileges

### PATH issues

If installed binaries can't be found:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

### .deb installation fails

- Check if the package is compatible with your system
- Ensure all dependencies are installed
- Try manual installation: `sudo dpkg -i package.deb`

## Uninstalling

To remove files installed by this tool:

- Binaries: `rm ~/.local/bin/<filename>`
- Libraries: `rm ~/.local/lib/<filename>`
- AppImages: `rm ~/.local/share/appimages/<filename>` and `rm ~/.local/bin/<symlink>`

To uninstall the installer itself:

```bash
make uninstall
```

## Building and Development

### Clean build

```bash
make clean
make
```

### Development targets

- `make all`: Build the installer (default)
- `make clean`: Remove build artifacts
- `make install`: Install to ~/.local/bin/
- `make uninstall`: Remove from ~/.local/bin/
- `make help`: Show available targets

## License

This project is open source. Please check the repository for license details.

## Contributing

Contributions are welcome! Please submit pull requests with:

- Clear description of changes
- Test cases for new functionality
- Documentation updates if needed

## Security Considerations

- The installer runs with user privileges by default
- .deb installation may require sudo privileges
- Always verify the source and integrity of packages before installation
- Be cautious when installing from untrusted sources