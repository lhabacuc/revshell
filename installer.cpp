#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

namespace fs = std::filesystem;

class PackageInstaller {
private:
    std::string homeDir;
    std::string localBinDir;
    std::string localLibDir;
    std::string appImageDir;

    void initializeDirectories() {
        struct passwd *pw = getpwuid(getuid());
        homeDir = pw->pw_dir;
        localBinDir = homeDir + "/.local/bin";
        localLibDir = homeDir + "/.local/lib";
        appImageDir = homeDir + "/.local/share/appimages";
        
        // Create directories if they don't exist
        fs::create_directories(localBinDir);
        fs::create_directories(localLibDir);
        fs::create_directories(appImageDir);
    }

    bool isDebPackage(const std::string& filename) {
        return filename.ends_with(".deb");
    }

    bool isAppImage(const std::string& filename) {
        return filename.ends_with(".AppImage") || filename.ends_with(".appimage");
    }

    bool isBinary(const std::string& filename) {
        if (isDebPackage(filename) || isAppImage(filename)) return false;
        
        // Check if file is executable
        struct stat st;
        if (stat(filename.c_str(), &st) == 0) {
            return (st.st_mode & S_IXUSR) != 0;
        }
        return false;
    }

    bool isLibrary(const std::string& filename) {
        return filename.ends_with(".so") || 
               filename.find(".so.") != std::string::npos ||
               filename.ends_with(".a");
    }

public:
    PackageInstaller() {
        initializeDirectories();
    }

    bool installDebPackage(const std::string& debFile) {
        std::cout << "Installing .deb package: " << debFile << std::endl;
        
        // Check if file exists
        if (!fs::exists(debFile)) {
            std::cerr << "Error: File does not exist: " << debFile << std::endl;
            return false;
        }

        // Try user-level installation first
        std::string cmd = "dpkg -i --force-not-root --root=" + homeDir + "/.local " + debFile;
        int result = system(cmd.c_str());
        
        if (result != 0) {
            std::cout << "User-level installation failed, trying with sudo..." << std::endl;
            cmd = "sudo dpkg -i " + debFile;
            result = system(cmd.c_str());
        }
        
        return result == 0;
    }

    bool installAppImage(const std::string& appImageFile) {
        std::cout << "Installing AppImage: " << appImageFile << std::endl;
        
        if (!fs::exists(appImageFile)) {
            std::cerr << "Error: File does not exist: " << appImageFile << std::endl;
            return false;
        }

        try {
            // Extract filename
            std::string filename = fs::path(appImageFile).filename();
            std::string destPath = appImageDir + "/" + filename;
            
            // Copy file
            fs::copy_file(appImageFile, destPath, fs::copy_options::overwrite_existing);
            
            // Make executable
            chmod(destPath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            
            // Create symlink in bin directory
            std::string linkPath = localBinDir + "/" + fs::path(filename).stem().string();
            fs::remove(linkPath); // Remove if exists
            fs::create_symlink(destPath, linkPath);
            
            std::cout << "AppImage installed successfully to: " << destPath << std::endl;
            std::cout << "Executable link created at: " << linkPath << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error installing AppImage: " << e.what() << std::endl;
            return false;
        }
    }

    bool installBinary(const std::string& binaryFile) {
        std::cout << "Installing binary: " << binaryFile << std::endl;
        
        if (!fs::exists(binaryFile)) {
            std::cerr << "Error: File does not exist: " << binaryFile << std::endl;
            return false;
        }

        try {
            std::string filename = fs::path(binaryFile).filename();
            std::string destPath = localBinDir + "/" + filename;
            
            // Copy file
            fs::copy_file(binaryFile, destPath, fs::copy_options::overwrite_existing);
            
            // Make executable
            chmod(destPath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            
            std::cout << "Binary installed successfully to: " << destPath << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error installing binary: " << e.what() << std::endl;
            return false;
        }
    }

    bool installLibrary(const std::string& libFile) {
        std::cout << "Installing library: " << libFile << std::endl;
        
        if (!fs::exists(libFile)) {
            std::cerr << "Error: File does not exist: " << libFile << std::endl;
            return false;
        }

        try {
            std::string filename = fs::path(libFile).filename();
            std::string destPath = localLibDir + "/" + filename;
            
            // Copy file
            fs::copy_file(libFile, destPath, fs::copy_options::overwrite_existing);
            
            std::cout << "Library installed successfully to: " << destPath << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error installing library: " << e.what() << std::endl;
            return false;
        }
    }

    bool autoInstall(const std::string& filename) {
        if (isDebPackage(filename)) {
            return installDebPackage(filename);
        } else if (isAppImage(filename)) {
            return installAppImage(filename);
        } else if (isLibrary(filename)) {
            return installLibrary(filename);
        } else if (isBinary(filename)) {
            return installBinary(filename);
        } else {
            std::cerr << "Error: Unknown file type: " << filename << std::endl;
            return false;
        }
    }

    void showHelp() {
        std::cout << "Ubuntu/Debian Package Installer\n";
        std::cout << "Usage: installer [options] <file1> [file2] ...\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help      Show this help message\n";
        std::cout << "  -d, --deb       Force install as .deb package\n";
        std::cout << "  -a, --appimage  Force install as AppImage\n";
        std::cout << "  -b, --binary    Force install as binary\n";
        std::cout << "  -l, --library   Force install as library\n";
        std::cout << "  -v, --verbose   Verbose output\n\n";
        std::cout << "Supported file types:\n";
        std::cout << "  - .deb packages (Debian/Ubuntu packages)\n";
        std::cout << "  - .AppImage files (Portable applications)\n";
        std::cout << "  - Binary executables\n";
        std::cout << "  - Libraries (.so, .a files)\n\n";
        std::cout << "Installation locations:\n";
        std::cout << "  - Binaries: ~/.local/bin/\n";
        std::cout << "  - Libraries: ~/.local/lib/\n";
        std::cout << "  - AppImages: ~/.local/share/appimages/\n";
        std::cout << "  - .deb packages: System-wide (requires sudo) or user-local\n";
    }
};

int main(int argc, char* argv[]) {
    PackageInstaller installer;
    
    if (argc < 2) {
        installer.showHelp();
        return 1;
    }

    std::vector<std::string> files;
    std::string forceType = "";
    bool verbose = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            installer.showHelp();
            return 0;
        } else if (arg == "-d" || arg == "--deb") {
            forceType = "deb";
        } else if (arg == "-a" || arg == "--appimage") {
            forceType = "appimage";
        } else if (arg == "-b" || arg == "--binary") {
            forceType = "binary";
        } else if (arg == "-l" || arg == "--library") {
            forceType = "library";
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            files.push_back(arg);
        }
    }

    if (files.empty()) {
        std::cerr << "Error: No files specified for installation\n";
        installer.showHelp();
        return 1;
    }

    bool allSuccess = true;
    
    for (const auto& file : files) {
        if (verbose) {
            std::cout << "\nProcessing file: " << file << std::endl;
        }
        
        bool success = false;
        
        if (forceType == "deb") {
            success = installer.installDebPackage(file);
        } else if (forceType == "appimage") {
            success = installer.installAppImage(file);
        } else if (forceType == "binary") {
            success = installer.installBinary(file);
        } else if (forceType == "library") {
            success = installer.installLibrary(file);
        } else {
            success = installer.autoInstall(file);
        }
        
        if (!success) {
            allSuccess = false;
            std::cerr << "Failed to install: " << file << std::endl;
        } else if (verbose) {
            std::cout << "Successfully installed: " << file << std::endl;
        }
    }

    return allSuccess ? 0 : 1;
}